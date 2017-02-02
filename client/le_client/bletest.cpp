#include <tinyb.hpp>
#include <tinyb/BluetoothException.hpp>

#include <thread>
#include <iostream>
#include <vector>

#include <atomic>
#include <csignal>

using namespace tinyb;

std::atomic<bool> running(true);

void signal_handler(int signum){
	if (signum == SIGINT){
		running = false;
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
		std::cerr << "Error while initializing libtintb: " << e.what() << std::endl;
		exit(1);
	}

	//start the discorvery of devices
	bool ret = manager->start_discovery();
	std::cout << "Started = " << (ret ? "true" : "false") << std::endl;

	BluetoothDevice *sensor_tag = NULL;
	BluetoothGattService *service = NULL;

	for(int i=0; i<15; i++){
		std::cout << "Discoverd devices: " << std::endl;
		auto list = manager->get_devices();

		for (auto it = list.begin(); it != list.end(); ++it){
			std::cout << "Class = " << (*it)->get_class_name() << " ";
			std::cout << "Path = " << (*it)->get_object_path() << " ";
			std::cout << "Name = " << (*it)->get_name() << " ";
			std::cout << "Connected = " << (*it)->get_connected() << " ";
			std::cout << std::endl;

			if((*it)->get_address() == argv[1])
				sensor_tag = (*it).release();
		}

		if (sensor_tag != nullptr)
			break;

		std::this_thread::sleep_for(std::chrono::seconds(4));
		std::cout << std::endl;
	}

	ret = manager->stop_discovery();
	std::cout << "Stopped = " << (ret ? "true":"false") << std::endl;

	if(sensor_tag == nullptr){
		std::cout << "Could not find device" << argv[1] << std::endl;
		return 1;
	}

	/* Connecte to the device and git services list */
	sensor_tag->connect();
	std::cout << "Discoverd services: " << std::endl;
	while(true){
		std::this_thread::sleep_for(std::chrono::seconds(4));

		auto list = sensor_tag->get_services();
		if (list.empty())
			continue;

		for (auto it = list.begin(); it != list.end(); it++){
			std::cout << "Class = " << (*it)->get_class_name() << " ";
			std::cout << "Path = " << (*it)->get_object_path() << " ";
			std::cout << "UUID = " << (*it)->get_device().get_object_path() << " ";
			std::cout << "Device = " << (*it)->get_device().get_object_path() << " ";
			std::cout << std::endl;

			/*Search service by UUID */
			if ((*it)->get_uuid() == argv[2])
				service = (*it).release();
		}
		break;
	}

	if (service == nullptr){
		std::cout << "Could not find service " << argv[2] << std::endl;
		return 1;
	}

	BluetoothGattCharacteristic *temp_value = nullptr;

	auto list = service->get_characteristics();
	std::cout << "Discoverd characteristics : " << std::endl;
	for (auto it = list.begin(); it != list.end(); it++){
		std::cout << "Class = " << (*it)->get_class_name() << " ";
		std::cout << "Path = " << (*it)->get_object_path() << " ";
		std::cout << "UUID = " << (*it)->get_uuid() << " ";
		std::cout << "Service = " << (*it)->get_service().get_object_path() << " ";
		std::cout << std::endl;

		if ((*it)->get_uuid() == argv[3])
			temp_value = (*it).release();
		
	}

	if (temp_value == nullptr){
		std::cout << "Could not find characteristics." << std::endl;
		return 1;
	}

	try{
		while (running) {
			std::vector<unsigned char> response = temp_value->read_value();
			unsigned char *data;
			unsigned int size = response.size();
			if(size > 0){
				data = response.data();
				std::cout << "data size : " << response.size() << std::endl;
				std::cout << "Raw data=[";
				for(unsigned i = 0; i < response.size(); i++)
					std::cout << std::hex <<static_cast<int>(data[i]) << ", ";
				std::cout << "] ";
			}
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}catch (std::exception &e){
		std::cout << "Error :  " << e.what() << std::endl;
	}

	/* disconnect from the device */
	try{
		sensor_tag->disconnect();
	} catch(std::exception &e){
		std::cout << "Error : " << e.what() << std::endl;
	}
	return 0;
}		
