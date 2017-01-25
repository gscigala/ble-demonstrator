//TODO : add licence

#include "ExternProc.h"


class BLEprocess: public ExternProcClient{
	public:
		//needs to be reimplemented
		virtual bool setup(int &argc, char **&argv);
		virtual bool procMain();

		virtual ~BLEprocess();
	
	protected:
		string getValue();
		list<string> scanDevices();
		void sendValues();

		virtual void readTimeout();
		virtual void messageReceived(const string &msg);
}
