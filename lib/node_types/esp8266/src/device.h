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
        void init(const char* subname, bool subscribed);
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
        // read from the globally defined device.
        // TODO: commented example
        #define ULNOIOT_ON_CHANGE_CALLBACK std::function<void()>
        ULNOIOT_ON_CHANGE_CALLBACK _on_change_cb=NULL;

        // This is the callback which is used for filtering and influencing values
        // It does not get the current device passed as parameter. The value can
        // be read from measured_value in the globally defined device. This
        // measure_value can be now modified. It is declared valid via
        // eturning true and invalid via returning flase or returning an empty 
        // string (set first char to 0).
        // TODO: commented example
        #define ULNOIOT_FILTER_CALLBACK std::function<bool()>
        ULNOIOT_FILTER_CALLBACK _filter_cb=NULL;

    protected:
        bool _started = false;

    public:
        Device(const char* _name);
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
                _on_change_cb();
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
        Device& with_on_change(ULNOIOT_ON_CHANGE_CALLBACK on_change_cb) { 
            return with_on_change_callback(on_change_cb);
        }
        Device& set_on_change(ULNOIOT_ON_CHANGE_CALLBACK on_change_cb) { 
            return with_on_change_callback(on_change_cb);
        }
        Device& with_filter_callback(ULNOIOT_FILTER_CALLBACK filter_cb) { 
            _filter_cb = filter_cb;
            return *this;
        }
        Device& set_filter_callback(ULNOIOT_FILTER_CALLBACK filter_cb) { 
            return with_filter_callback(filter_cb);
        }
        Device& with_filter(ULNOIOT_FILTER_CALLBACK filter_cb) { 
            return with_filter_callback(filter_cb);
        }
        Device& set_filter(ULNOIOT_FILTER_CALLBACK filter_cb) { 
            return with_filter_callback(filter_cb);
        }

        Subdevice* add_subdevice(Subdevice* sd) {
            ulog("add_subdevice: dev: %s sd: %s",name.as_cstr(), sd->get_name().as_cstr());
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

        bool started() { return _started; }

        /* poll_measure
         * This calls measure and filters and sets if necessary default values. 
         * This will be called very often from event loop
         * Will return if current measurement was successful or invalid. 
         * */
        bool poll_measure();

        /* check_changes
         * returns True if new values were measured and sets the publishing flag
         * This will be called very often from event loop 
         * This only reports change when the measured (and filtered) 
         * value is new (has changed) and report_changes is set.
         * If a change happened and a callback is set, the callback is called.
         * */
        bool check_changes();

        /* measure_init
         * Can be overloaded to allow re-initialization of some hardware
         * elements. For example a respective i2c bus due to a very weird
         * implementation of TwoWire in Wire.h
         * */
        virtual void measure_init() {}; // usually do nothing here

        /* start
         * This usually needs to be overwritten
         * Any physical initialization has to go here.
         * This is executed by the system right after all the rest has been 
         * intialized - right before the first subscription.
         * If something with the physical initialization goes wrong, started
         * should stay false. If physical intitialization worked this method
         * needs to set _started to true;
         * TODO: make protected?
         * */
        virtual void start() { _started = true; }

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
         * If device is not marked as started, measure will not be called.
         * TODO: make protected?
         * */
        virtual bool measure() { return true; }
};


//// Some filters
// TODO: check how to turn this in a functor class that does not die when used
#define filter_average(TYPE, buflen, dev) [&] { \
        static TYPE sum = 0; \
        static size_t values_count = 0; \
        TYPE v; \
        if(std::is_same<TYPE, double>::value) { \
            v = dev.measured_value().as_float(); \
        } else { \
            v = dev.measured_value().as_int(); \
        } \
        sum += v; \
        values_count ++; \
        if(values_count >= buflen) { \
            dev.measured_value().from(sum/values_count); \
            values_count = 0; \
            sum = 0; \
            return true; \
        } \
        return false; \
    }



/* The Jeff McClintock running median estimate. 
 * base: https://stackoverflow.com/questions/10930732/c-efficiently-calculating-a-running-median
 * */
#define filter_jmc_median(update_ms, dev) [&] { \
        static double median = 0.0; \
        static double average = 0.0; \
        static unsigned long last_time = millis(); \
        double sample = dev.measured_value().as_float(); \
        average += ( sample - average ) * 0.1; \
        median += copysign( average * 0.01, sample - median ); \
        unsigned long current = millis() ; \
        if(current - last_time >= update_ms) { \
            dev.measured_value().from(average); \
            last_time = current; \
            return true; \
        } \
        return false; \
    }


#endif // _ULNOIOT_DEVICE_H_
