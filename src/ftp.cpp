#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include "ftp.h"
using namespace std;

/* NOTE: if you want this example to work on Windows with libcurl as a
   DLL, you MUST also provide a read callback with CURLOPT_READFUNCTION.
   Failing to do so will give you a crash since a DLL may not use the
   variable's memory when passed in to it from an app like this. */
size_t ftp::read_callback(char *ptr, size_t size, size_t nmemb, FILE *stream)
{
	/* in real-world cases, this would probably get this data differently
	   as this fread() stuff is exactly what the library already would do
	   by default internally */
	unsigned long nread;
	size_t retcode = fread(ptr, size, nmemb, stream);

	if(retcode > 0) {
		nread = (unsigned long)retcode;
	}

	return retcode;
}

/*
 * Passata una path come argomento, ritorna il nome del file.
 * Es: /foo/bar/moo.txt
 * ritorna moo.txt
 */
void ftp::get_local_file_name(char* n, const char* path) {
	int i = strlen(path) - 1;
	for (; i >=0; i--) {
		if (path[i] == '/') break;
	}
	unsigned int pos = 0;
	for (unsigned int j = i+1; j < strlen(path); j++) {
		n[pos++] = path[j];
	}
	n[pos] = '\0';
}

/*
 * Unisce due stringhe passate come argomento.
 */
void ftp::combine_string(char* n, const char* str1, const char* str2) {
	unsigned int pos = 0;
	for (unsigned int i = 0; i < strlen(str1); i++) {
		n[pos++] = str1[i];
	}
	for (unsigned int i = 0; i < strlen(str2); i++) {
		n[pos++] = str2[i];
	}
	n[pos] = '\0';
}

int ftp::uploadFile(const char* f) {

	CURL *curl;
	CURLcode res;
	/* get a curl handle */
	curl = curl_easy_init();
	if (!curl) {
		std::cout << "Error. Exit\n";
		return 1;
	}

	curl_global_init(CURL_GLOBAL_ALL);
	/* we want to use our own read function */
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, ftp::read_callback);
	/* setta lo username e la password */
	char un[100];
	createUserName(un);
	curl_easy_setopt( curl, CURLOPT_USERPWD, un );

	// apre il file da caricare
	FILE* source_file = fopen (f, "rb");
	// memorizza il nome del file
	char file_name[100];
	get_local_file_name(file_name, f);
	if (verbose) std::cout << "Upload file " << file_name << " to ";
	/* enable uploading */
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
	/* crea il nome della URL remota */
	char remote_url[100];
	combine_string(remote_url, ftp_host, file_name);
	if (verbose) std::cout << remote_url << "\n";
	/* specify target */
	curl_easy_setopt(curl, CURLOPT_URL, remote_url);

	/* [....] */
	/* now specify which file to upload */
	curl_easy_setopt(curl, CURLOPT_READDATA, source_file);

	char hl1[100];
	combine_string(hl1, "RNFR ", file_name);


	struct curl_slist *headerlist = NULL;
	/* build a list of commands to pass to libcurl */
	headerlist = curl_slist_append(headerlist, hl1);
	//static const char buf_2 [] = "RNTO " RENAME_FILE_TO;
	// headerlist = curl_slist_append(headerlist, buf_2);
	/* pass in that last of FTP commands to run after the transfer */
	curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);

	/* Now run off and do what you have been told! */
	res = curl_easy_perform(curl);
	/* Check for errors */
	if(res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
		// chiude il file
		fclose(source_file);
		/* clean up the FTP commands list */
		curl_slist_free_all(headerlist);

		/* always cleanup */
		curl_easy_cleanup(curl);
		curl_global_cleanup();
		return 1;
	}
	// chiude il file
	fclose(source_file);
	/* clean up the FTP commands list */
	curl_slist_free_all(headerlist);

	/* always cleanup */
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return 0;
}

int ftp::downloadFile(const char* local_file) {
	char file_name[100];
	get_local_file_name(file_name, local_file);
	return downloadFile(file_name, local_file);
}

int ftp::downloadFile(const char* ftp_file, const char* local_file) {
	CURL *curl;
	CURLcode res;
	struct FtpFile ftpfile = {
		local_file, NULL
	};

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();
	if(curl) {
		/*
		 * You better replace the URL with one that works!
		 */
		char remote_file[100];
		combine_string(remote_file, ftp_host, ftp_file);
		curl_easy_setopt(curl, CURLOPT_URL, remote_file);
		char un[100];
		createUserName(un); 
		curl_easy_setopt( curl, CURLOPT_USERPWD, un);
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

	curl_global_cleanup();
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
	this->password = strdup(pwd.c_str());
}

void ftp::setUserName(std::string userName) {
	this->userName = strdup(userName.c_str());
}

void ftp::setHost(std::string ftp_host) {
	this->ftp_host = strdup(ftp_host.c_str());
}

void ftp::setVerbose(bool v) {
	verbose = v;
}

void ftp::createUserName(char* userName) {
	strcpy(userName, this->userName);
	strcat(userName, ":");
	strcat(userName, password);
}

void ftp::free_memory() {
	free(ftp_host);
	free(userName);
	free(password);
}
