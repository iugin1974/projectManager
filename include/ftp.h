#ifndef FTP_H
#define FTP_H

#include <string>
class ftp {
	private:
		std::string password;
		std::string userName;
		std::string ftp_fileName;
		bool verbose = false;
		bool passwordSetted = false;
		std::string ftp_host;
		static size_t read_callback(char *ptr, size_t size, size_t nmemb, FILE *stream);
		std::string combine_string(const std::string& str1, const std::string& str2);
		std::string get_local_file_name(const std::string&); 
		static size_t my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream);
		struct FtpFile {
			const char *filename;
			FILE *stream;
		};
		std::string createUserName();
	public:
		static void init();
		static void close();
		int uploadFile(const std::string& f);
		int downloadFile(const std::string& ftp_file, const std::string& local_file);
		int downloadFile(const std::string& local_file);
		bool isPasswordSet();
		void setPassword(std::string pwd);
		void setVerbose(bool v);
		void setHost(std::string  ftp_host);
		void setUserName(std::string userName);

};
#endif
