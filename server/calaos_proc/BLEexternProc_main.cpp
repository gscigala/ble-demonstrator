//TODO : add licence

#include "ExternProc.h"


class BLEprocess: public ExternProcClient{
	public:
		//needs to be reimplemented
		virtual bool setup(int &argc, char **&argv);
		virtual bool procMain();

		virtual ~BLEprocess();
	
	protected:
		int getValue(const string &device);
		list<string> scanDevices();
		void sendValues();

		virtual void readTimeout();
		virtual void messageReceived(const string &msg);
}

int BLEprocess::getValue(){
	//TODO
}

void BLEprocess::readTimeout{
	lisT<string> l = scanDevices();

	Json jdata;
	for (const string &dev: l){
		jdata.push_back({
			{"id", dev},
			{"value", getValue(dev) },
		});
  	}

	sendMessage(jdata.dump());	
}	
