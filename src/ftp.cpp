#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <filesystem>
#include "ftp.h"
using namespace std;

void ftp::init() {
    curl_global_init(CURL_GLOBAL_ALL);
}

void ftp::close() {
    curl_global_cleanup();
}
/* NOTE: if you want this example to work on Windows with libcurl as a
DLL, you MUST also provide a read callback with CURLOPT_READFUNCTION.
Failing to do so will give you a crash since a DLL may not use the
variable's memory when passed in to it from an app like this. */
size_t ftp::read_callback(char *ptr, size_t size, size_t nmemb, FILE *stream)
{
/* in real-world cases, this would probably get this data differently
   as this fread() stuff is exactly what the library already would do
   by default internally */
size_t retcode = fread(ptr, size, nmemb, stream);
return retcode;
}

/*
 * Passata una path come argomento, ritorna il nome del file.
 * Es: /foo/bar/moo.txt
 * ritorna moo.txt
 */
std::string ftp::get_local_file_name(const std::string& n) {
	return std::filesystem::path(n).filename().string();
}

/*
 * Unisce due stringhe passate come argomento.
 */
std::string ftp::combine_string(const std::string& str1, const std::string& str2) {
	return str1 + str2;
}

int ftp::uploadFile(const std::string& f) {

	CURL *curl;
	CURLcode res;
	/* get a curl handle */
	curl = curl_easy_init();
	if (!curl) {
		std::cout << "Error. Exit\n";
		return 1;
	}

	/* setta lo username e la password */
	std::string un = createUserName();
	curl_easy_setopt( curl, CURLOPT_USERPWD, un.c_str());

	// apre il file da caricare
	FILE* source_file = fopen (f.c_str(), "rb");
	// memorizza il nome del file
	std::string file_name = get_local_file_name(f);
	if (verbose) std::cout << "Upload file " << file_name << " to ";
	/* enable uploading */
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
	/* crea il nome della URL remota */
	std::string remote_url = combine_string(ftp_host, file_name);
	if (verbose) std::cout << remote_url << "\n";
	/* specify target */
	curl_easy_setopt(curl, CURLOPT_URL, remote_url.c_str());

	/* [....] */
	/* now specify which file to upload */
	curl_easy_setopt(curl, CURLOPT_READDATA, source_file);

	/* Now run off and do what you have been told! */
	res = curl_easy_perform(curl);
	/* Check for errors */
	if(res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
		// chiude il file
		fclose(source_file);
		/* clean up the FTP commands list */

		/* always cleanup */
		curl_easy_cleanup(curl);
		return 1;
	}
	// chiude il file
	fclose(source_file);
	/* clean up the FTP commands list */

	/* always cleanup */
	curl_easy_cleanup(curl);
	return 0;
}

int ftp::downloadFile(const std::string& local_file) {
	std::string file_name = get_local_file_name(local_file);
	return downloadFile(file_name, local_file);
}

int ftp::downloadFile(const std::string& ftp_file, const std::string& local_file) {
	CURL *curl;
	CURLcode res = CURLE_FAILED_INIT;
	struct FtpFile ftpfile = {
		local_file.c_str(), NULL
	};


	curl = curl_easy_init();
	if(curl) {
		std::string remote_file = combine_string(ftp_host.c_str(), ftp_file);
		curl_easy_setopt(curl, CURLOPT_URL, remote_file.c_str());
		std::string un = createUserName(); 
		curl_easy_setopt( curl, CURLOPT_USERPWD, un.c_str());
		/* Define our callback to get called when there's data to be written */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);
		/* Set a pointer to our struct to pass to the callback */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);

		/* Switch on full protocol/debug output */
		if (verbose)
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		res = curl_easy_perform(curl);

		/* always cleanup */
		curl_easy_cleanup(curl);

		if(CURLE_OK != res) {
			/* we failed */
			fprintf(stderr, "curl told us %d\n", res);
		}
	}

	if(ftpfile.stream)
		fclose(ftpfile.stream); /* close the local file */

	return res;
}

size_t ftp::my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
	struct FtpFile *out = (struct FtpFile *)stream;
	if(!out->stream) {
		/* open file for writing */
		out->stream = fopen(out->filename, "wb");
		if(!out->stream)
			return -1; /* failure, cannot open file to write */
	}
	return fwrite(buffer, size, nmemb, out->stream);
}


void ftp::setPassword(std::string pwd) {
	this->password = pwd;
	passwordSetted = true;
}

bool ftp::isPasswordSet() {
return passwordSetted;
}

void ftp::setUserName(std::string userName) {
	this->userName = userName;
}

void ftp::setHost(std::string ftp_host) {
	this->ftp_host = ftp_host;
}

void ftp::setVerbose(bool v) {
	verbose = v;
}

std::string ftp::createUserName() {
	return this->userName+":"+password;
}
