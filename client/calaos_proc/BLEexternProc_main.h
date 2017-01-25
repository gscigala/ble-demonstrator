#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include <stdlib>


typedef struct hci_state{
	int device_id;
	int device_handle;
	struct hci_filter original_filter;
	int state;
	bool has_error;
	string error_message;
} hci_state;

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

		hci_state open_default_hci_device();
}

