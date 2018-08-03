// device.h
// author: ulno
// created: 2018-07-16
//
// One ulnoiot device (actor or sensor connected to node)

// General note, in ulnoiot, we pass as convenience all values as strings.
// So other types have to be converted in such a as string or from one.

#ifndef _ULNOIOT_DEVICE_H_
#define _ULNOIOT_DEVICE_H_

#include <functional>
#include <AsyncMqttClient.h>

#include <ulnoiot-default.h>
#include <toolbox.h>



class Subdevice {
    private:
        Ustring name; // subdevice name (added to the device name)
        bool _subscribed = false;
        void init(const char* subname, bool subscribed) {
            name.from(subname);
            _subscribed = subscribed;
        }
        #define ULNOIOT_ON_RECEIVE_CALLBACK std::function<bool(const Ustring&)>
        ULNOIOT_ON_RECEIVE_CALLBACK receive_cb = NULL;
    public:
        Ustring measured_value; // the just measured value (after calling measure)
        Ustring current_value;
        Ustring& value() { return current_value; }
        Ustring& get() { return current_value; }
        Subdevice& with_receive_cb(ULNOIOT_ON_RECEIVE_CALLBACK cb) {
            receive_cb = cb;
            return *this;
        }
        bool call_receive_cb(Ustring& payload);
        const Ustring& get_name() const { return name; }
        const Ustring& key() const { return name; }
        bool subscribed() { return _subscribed; }
        Subdevice(const char* subname, bool subscribed) {
            init(subname, subscribed);
        }
        Subdevice(const char* subname) { 
            init(subname, false);
        }
};

class Device {
    protected:
        Fixed_Map<Subdevice, ULNOIOT_MAX_SUBDEVICES> subdevices;
    private:
        Ustring name; // device name and mqtt-topic extension
        bool _ignore_case = true;
        bool _report_change = true;
        bool _needs_publishing = false;

        // This is the callback which is called based on a value change
        // it gets passed the triggering device. Last measured values can be
        // read from the device.
        // TODO: commented example
        #define ULNOIOT_ON_CHANGE_CALLBACK std::function<void(Device&)>
        ULNOIOT_ON_CHANGE_CALLBACK _on_change_cb=NULL;

        // This is the callback which is used for filtering and influencing values
        // It gets the current device passed as parameter. Values
        // modify. So it returns the same or modified value in it. If it wants to
        // invalidate the current measurement, it needs to return an empty string (
        // set first char to 0) or return false. To indicate change or validate the
        // measured value it needs to return true.
        // TODO: commented example
        #define ULNOIOT_FILTER_CALLBACK std::function<bool(Device&)>
        ULNOIOT_FILTER_CALLBACK _filter_cb=NULL;

    public:
        Device(const char* _name) { name.from(_name); }
        //// Getters & Setters
        Device& with_ignore_case(bool ignore_case) { 
            _ignore_case = ignore_case;
            return *this;
        }
        Device& set_ignore_case(bool ignore_case) {
            return with_ignore_case(ignore_case);
        }

        bool get_ignore_case() {
            return _ignore_case;
        }

        void call_on_change_callback() {
            if(_on_change_cb != NULL) {
                _on_change_cb(*this);
            }
        }

        bool needs_publishing() {
            return _needs_publishing;
        }

        // publish current value(s) and resets needs_publishing state
        bool publish(AsyncMqttClient& mqtt_client, Ustring& node_topic);

        Ustring& value(unsigned long index);
        Ustring& value() { return value(0); }
        Ustring& measured_value(unsigned long index);
        Ustring& measured_value() { return measured_value(0); }
        const Ustring& get_name() const { return name; }
        //const char* get_name() const { return name.as_cstr(); }
        const Ustring& key() const { return name; }
        
        virtual ~Device() {
            // usually nothing has to be done here
            // This virtual method needs to be defined to prevent warning
            // from cpp-compiler
        }

        Device& with_report_change(bool report_change) { 
            _report_change = report_change;
            return *this;
        }
        Device& set_report_change(bool report_change) {
            return with_report_change(report_change);
        } 
        Device& with_on_change_callback(ULNOIOT_ON_CHANGE_CALLBACK on_change_cb) { 
            _on_change_cb = on_change_cb;
            return *this;
        }
        Device& set_on_change_callback(ULNOIOT_ON_CHANGE_CALLBACK on_change_cb) { 
            return with_on_change_callback(on_change_cb);
        }
        Device& with_filter_callback(ULNOIOT_FILTER_CALLBACK filter_cb) { 
            _filter_cb = filter_cb;
            return *this;
        }
        Device& set_filter_callback(ULNOIOT_FILTER_CALLBACK filter_cb) { 
            return with_filter_callback(filter_cb);
        }

        Subdevice* add_subdevice(Subdevice* sd) {
            if(subdevices.add(sd)) {
                return sd;
            } else {
                return NULL;
            }
        }

        Subdevice* subdevice(unsigned long index) {
            return subdevices.get(index);
        }

        bool subdevices_for_each(std::function<bool(Subdevice&)> func) {
            return subdevices.for_each(func);
        }

        /* measure
         * Does nothing by default.
         * Usually this needs to be overwritten.
         * It should make sure to trigger necessary steps to
         * measure from the physical hardware a sensor value (or
         * several values from a multi sensor).
         * It needs to set in measured_value the currently measured value.
         * If no value can be measured (or has not been), it should return
         * false. If measuring was successful, return True.
         * This is called from update to trigger the actual value generation.
         */
        virtual bool measure() { return true; }

        /* poll_measure
         * This calls measure and and filters and sets if necessary default values. 
         * This will be called very often from event loop
         * Will return if current measurement was successful or invalid. 
         */
        bool poll_measure();

        /* check_changes
         * returns True if new values were measured and sets the publishing flag
         * This will be called very often from event loop 
         * This only reports change when the measured (and filtered) 
         * value is new (has changed) and report_changes is set.
         * If a change happened and a callback is set, the callback is called.
         */
        bool check_changes();
};

#endif // _ULNOIOT_DEVICE_H_
