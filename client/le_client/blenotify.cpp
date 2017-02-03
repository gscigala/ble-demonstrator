#include <tinyb.hpp>
#include <tinyb/BluetoothException.hpp>

#include <thread>
#include <iostream>
#include <vector>

#include <csignal>
#include <condition_variable>

using namespace tinyb;

std::condition_variable cv;

void data_callback(BluetoothGattCharacteristic &c, std::vector<unsigned char> &data, void *userdata){
	/* Read temperature data and display it */
	unsigned char *data_c;
	unsigned int size = data.size();
	if(size > 0){
		data_c = data.data();
		std::cout << "Raw data=[";
		for (unsigned i = 0; i < size; i++){
			std::cout << std::hex << static_cast<int>(data_c[i]) << ", ";
		}
		std::cout << "] " << std::endl;
	}
} 

void signal_handler(int signum){
	if (signum == SIGINT){
		cv.notify_all();
	}
}

int main(int argc, char **argv){

	if(argc < 4){
		std::cerr << "Run as " << argv[0] << " <device_address> <service_UUID> <characteristics_UUID>" << std::endl;
		exit(1);
	}

	BluetoothManager *manager = nullptr;
	try{
		manager = BluetoothManager::get_bluetooth_manager();
	}catch(const std::runtime_error& e){
		std::cerr << "Error while initializing libtinyb: " << e.what() << std::endl;
		exit(1);
	}

	//start the discorvery of devices
	bool ret = manager->start_discovery();
	std::cout << "Started = " << (ret ? "true" : "false") << std::endl;
	
	std::unique_ptr<BluetoothGattService> service;
	
	std::string device_mac(argv[1]);

	auto sensor_tag = manager->find<BluetoothDevice>(nullptr, &device_mac, nullptr, std::chrono::seconds(10));

	if(sensor_tag == nullptr){
		std::cout << "Device not found" << std::endl;
		return 1;
	}

	sensor_tag->enable_connected_notifications([] (BluetoothDevice &d, bool connected, void *userdata)
		{ if (connected) std::cout << "Connected" << d.get_name() << std::endl; }, NULL);

	if (sensor_tag != nullptr){	
		/* Connect to the device and git services list */
		sensor_tag->connect();
		std::string service_uuid = std::string(argv[2]);

		std::cout << "Waiting for service" << service_uuid << "to be discovered" << std::endl;
		service = sensor_tag->find(&service_uuid);
	} else{
		ret = manager->stop_discovery();
		std::cerr << "SensorTag not found after 30 seconds, exiting" << std::endl;
		return 1;
	}

	/* Stop the discovery (the device was found or timeout was over) */
	ret = manager->stop_discovery();
	std::cout << "Stopped = " << (ret ? "true" : "false") << std::endl;

	auto value_uuid = std::string(argv[3]);
	auto temp_value = service->find(&value_uuid);

	/*Activate mesurements */
	temp_value->enable_value_notifications(data_callback, nullptr);
	
	std::mutex m;
	std::unique_lock<std::mutex> lock(m);

	std::signal(SIGINT, signal_handler);
	cv.wait(lock);
	
	/* disconnect from the device */
	try{
		sensor_tag->disconnect();
	} catch(std::exception &e){
		std::cout << "Error : " << e.what() << std::endl;
	}
	return 0;
}		
