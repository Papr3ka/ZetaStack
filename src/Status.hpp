#ifndef STATUS_HPP
#define STATUS_HPP

namespace bar {

	void setstate(bool mode);

	void init(long int max);

	void start(void);

	void stop(void);

	void join(void);

	void inform(std::string md);

	void changemode(int m);

	void setcount(float x);

	void updatecycle(void);

	void updatepercent(void);

	void finish(void);

	void barmanager(void);
}

#endif