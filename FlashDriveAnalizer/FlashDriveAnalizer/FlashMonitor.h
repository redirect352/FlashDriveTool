#pragma once
#include <memory>
#include <stdexcept>
#include <set>
#include <map>
#include <string>
#include <Windows.h>
#include <cfgmgr32.h>
#include <boost/function.hpp>

class flashMonitorExeption : public std::runtime_error
{
public:
	explicit flashMonitorExeption(const std::string& message);
};

class flashMonitor
{
public:
	static flashMonitor* create(HINSTANCE hinst,bool monitor_hard_drives = false);
	static void remove();

	template<typename Handler>
	void on_device_add(Handler handler)
	{
		on_device_added_ = handler;
	}

	template<typename Handler>
	void on_device_remove(Handler handler)
	{
		on_device_removed_ = handler;
	}

	template<typename Handler>
	void on_device_safe_remove(Handler handler)
	{
		on_device_safe_removed_ = handler;
	}

	template<typename Handler>
	void on_device_remove_fail(Handler handler)
	{
		on_device_remove_failed_ = handler;
	}

	void on_device_add();
	void on_device_remove();
	void on_device_safe_remove();
	void on_device_remove_fail();

	void start();
	void stop();
	bool is_started() const;

	static std::set<wchar_t> get_flash_disks(bool include_usb_hard_drives);

	void mount_existing_devices();

	void unmount_all_devices();

	void safe_eject(char letter);

	void set_safe_remove_on_timeout(bool remove);

	bool is_set_safe_remove_on_timeout_on() const;

	void enable_usb_hard_drive_monitoring(bool enable);

	bool is_usb_hard_drive_monitoring_enabled() const;

	BOOL devices_changed(WPARAM wParam, LPARAM lParam);


	static void message_pump();

	~flashMonitor();

private:
	explicit flashMonitor(HINSTANCE hInstanse, bool monitor_hard_drives = false);
	flashMonitor(const flashMonitor&);
	flashMonitor& operator=(const flashMonitor&);

	bool started_, safe_remove_on_timeout_, monitor_hard_drives_;
	HWND mon_hwnd_;

	std::set<wchar_t> existing_usb_devices_;
	typedef std::map<size_t, std::pair<HDEVNOTIFY, wchar_t> > notifications;
	notifications existing_notifications_;

	static INT_PTR WINAPI WinProcCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static std::auto_ptr<flashMonitor> instanse_;

	void detect_changed_devices();
	void mount_device(wchar_t letter);
	void unmount_device(wchar_t letter, bool call_unsafe_callback = true);
	
	struct device_info
	{
		DEVINST dev_inst;
		GUID dev_class;
		long dev_number;
	};

	static const device_info get_device_info(char letter);

	boost::function<void(char)> on_device_added_;
	boost::function<void(char)> on_device_removed_;
	boost::function<bool(char)> on_device_safe_removed_;
	boost::function<void(char)> on_device_remove_failed_;

	static const std::wstring class_name;
};
