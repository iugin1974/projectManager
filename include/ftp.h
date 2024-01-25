#ifndef FTP_H
#define FTP_H

#include <string>
class ftp {
	private:
		char* password;
		char* userName;
		char* ftp_fileName;
		bool verbose = false;
		bool passwordSetted = false;
		char* ftp_host;
		static size_t read_callback(char *ptr, size_t size, size_t nmemb, FILE *stream);
		void combine_string(char* n, const char* str1, const char* str2);
		void get_local_file_name(char* n, const char* path); 
		static size_t my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream);
		struct FtpFile {
			const char *filename;
			FILE *stream;
		};
		void createUserName(char* userName);
	public:
		int uploadFile(const char* f);
		int downloadFile(const char* ftp_file, const char* local_file);
		int downloadFile(const char* local_file);
		bool isPasswordSet();
		void setPassword(std::string pwd);
		void setVerbose(bool v);
		void setHost(std::string  ftp_host);
		void setUserName(std::string userName);
		void free_memory();

};
#endif
