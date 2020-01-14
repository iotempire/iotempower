// device.h
// author: ulno
// created: 2018-07-16
//
// One iotempower device (actor or sensor connected to node)

// General note, in iotempower, we pass as convenience all values as strings.
// So other types have to be converted into a string or from one.
// We are actually using Ustring as defined in toolbox.h.

#ifndef _IOTEMPOWER_DEVICE_H_
#define _IOTEMPOWER_DEVICE_H_

#include <functional>
////AsyncMqttClient disabled in favor of PubSubClient
//#include <AsyncMqttClient.h>
#include <PubSubClient.h>
#include <iotempower-default.h>
#include <toolbox.h>

#include "config-wrapper.h"

class Device;

#define IOTEMPOWER_DEVICE_CALLBACK std::function<bool(Device&)>

// This class implements a callback for IOTEMPOWER
// It's call function can be overwritten or the internal callback
// can be intialized by another function
class Callback {
    private:
        IOTEMPOWER_DEVICE_CALLBACK _callback = NULL;
    public:
        Callback(IOTEMPOWER_DEVICE_CALLBACK callback = NULL) {
            _callback = callback;
        }
        virtual bool call(Device& device) {
            if(_callback) return _callback(device);
            return false; // if not overwritten, let this filter forget everything
        }
};


// This is a chained list of callbacks working on a device
// If called it traverses the chain and all it's subchain elements and
// returns only true if all subcalls returned true - else it will return false
// the call back operates on a device as parameter
class Callback_Link {
    private:
        Callback* _callback;
        Callback_Link* next = NULL; // allow building chain
    public:
        Callback_Link(Callback& cb) {
            _callback = &cb;
        }
        bool call(Device &dev) {
            bool result = _callback->call(dev);
            if(result && next) { // traverse chain down to end
                result &= next->call(dev);
            }
            return result;
        }
        bool chain(Callback& cb) {
            if(next) {
                return next->chain(cb); // traverse chain down to end
            } else {
                next = new Callback_Link(cb);
                return next != NULL;
            }
        }
};

class Subdevice {
    private:
        Ustring name; // subdevice name (added to the device name)
        bool _subscribed = false;
        void init_log();
        void init(const char* subname, bool subscribed);
        void init(const __FlashStringHelper* subname, bool subscribed);
        #define IOTEMPOWER_ON_RECEIVE_CALLBACK std::function<bool(const Ustring&)>
        IOTEMPOWER_ON_RECEIVE_CALLBACK receive_cb = NULL;
    public:
        Ustring measured_value; // the just measured value (after calling measure)
        Ustring current_value;
        Ustring& value() { return current_value; }
        Ustring& get() { return current_value; }
        Subdevice& with_receive_cb(IOTEMPOWER_ON_RECEIVE_CALLBACK cb) {
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
        Subdevice(const __FlashStringHelper* subname, bool subscribed) {
            init(subname, subscribed);
        }
        Subdevice(const __FlashStringHelper* subname) { 
            init(subname, false);
        }
};

class Device {
    protected:
        Fixed_Map<Subdevice, IOTEMPOWER_MAX_SUBDEVICES> subdevices;
#ifdef mqtt_discovery_prefix
        // TODO: might have to move this to subdevice (if there are two values measured)
        String discovery_config_topic;
        String discovery_info;
        void create_discovery_info(const String& type,
            bool state_topic=true, const char* state_on=NULL, const char* state_off=NULL,
            bool command_topic=false, const char* payload_on=NULL, const char* payload_off=NULL,
            const String& extra_json=String());
#endif
    private:
        Ustring name; // device name and mqtt-topic extension
        bool _ignore_case = true;
        bool _report_change = true;
        bool _needs_publishing = false;


        // This is a chain of callbacks called for every measuring iteration of the
        // device.
        // It gets passed the triggering device. 
        // the last measured value will be in .measured_value() or
        // .measured_value(n) with n denoting the subdevice.
        // .measured_value can be modified.
        // Returning false means that the value should not be considered.
        // Returning true means that the (new) value in .measured_value is correct.
        // TODO: commented example
        Callback_Link *filter_first;


        // This is a chain of callbacks which are called based on
        // a value change of the or one of any of the values of the subdevices.
        // It gets passed the triggering device. 
        // the last measured value will be in .measured_value() or
        // .measured_value(n) with n denoting the subdevice.
        // Returning false also allows here to discard the value.
        // Returning true menas that value was considered.
        // TODO: commented example
        Callback_Link *on_change_first;

        unsigned long _pollrate_us = 0; // poll as often as possible
        unsigned long last_poll = micros(); // when was last polled

    protected:
        bool _started = false;

    public:
        Device(const char* _name, unsigned long pollrate_us);
        //// Getters & Setters
        unsigned int subdevices_count() {
            return subdevices.length();
        }
        Device& with_ignore_case(bool ignore_case) { 
            _ignore_case = ignore_case;
            return *this;
        }
        Device& set_ignore_case(bool ignore_case) {
            return with_ignore_case(ignore_case);
        }
        Device& ignore_case(bool ignore_case) {
            return with_ignore_case(ignore_case);
        }

        bool get_ignore_case() {
            return _ignore_case;
        }


        ///////// Publishing
        bool needs_publishing() {
            return _needs_publishing;
        }

        // publish current value(s) and resets needs_publishing state
        ////AsyncMqttClient disabled in favor of PubSubClient
        //bool publish(AsyncMqttClient& mqtt_client, Ustring& node_topic);
        bool publish(PubSubClient& mqtt_client, Ustring& node_topic);

#ifdef mqtt_discovery_prefix
        ////AsyncMqttClient disabled in favor of PubSubClient
        // bool publish_discovery_info(AsyncMqttClient& mqtt_client);
        bool publish_discovery_info(PubSubClient& mqtt_client);
#endif

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


        //// pollrate
        Device& set_pollrate_us(unsigned long rate_us) { 
            _pollrate_us = rate_us;
            return *this;
        }
        Device& with_pollrate_us(unsigned long rate_us) { 
            return set_pollrate_us(rate_us);
        }
        Device& pollrate_us(unsigned long rate_us) { 
            return set_pollrate_us(rate_us);
        }
        Device& set_pollrate(unsigned long rate_ms) { 
            return set_pollrate_us(rate_ms*1000);
        }
        Device& with_pollrate(unsigned long rate_ms) { 
            return set_pollrate(rate_ms);
        }
        Device& pollrate(unsigned long rate_ms) { 
            return set_pollrate(rate_ms);
        }
        unsigned int get_pollrate() {
            return _pollrate_us/1000;
        }
        unsigned int get_pollrate_us() {
            return _pollrate_us;
        }

        //// report_change
        Device& set_report_change(bool report_change) { 
            _report_change = report_change;
            return *this;
        }
        Device& with_report_change(bool report_change) {
            return set_report_change(report_change);
        } 
        Device& report_change(bool report_change=true) {
            return set_report_change(report_change);
        } 

        //// on_change_callback
        Device& set_on_change_callback(Callback& on_change_cb) {
            bool result;
            if(on_change_first) result=on_change_first->chain(on_change_cb);
            else {
                on_change_first = new Callback_Link(on_change_cb);
                result = on_change_first != NULL;
            }
            if(!result) {
                ulog(F("Trouble reserving on_change entry."));
            }
            return *this;
        }
        Device& with_on_change_callback(Callback& on_change_cb) { 
            return set_on_change_callback(on_change_cb);
        }
        Device& on_change_callback(Callback& on_change_cb) { 
            return set_on_change_callback(on_change_cb);
        }
        Device& set_on_change(Callback& on_change_cb) { 
            return set_on_change_callback(on_change_cb);
        }
        Device& with_on_change(Callback& on_change_cb) { 
            return set_on_change_callback(on_change_cb);
        }
        Device& on_change(Callback& on_change_cb) { 
            return set_on_change_callback(on_change_cb);
        }
        bool call_on_change_callbacks() {
            if(on_change_first) {
                return on_change_first->call(*this);
            } else {
                return true; // all ok if there are no on_change callbacks
            }
        }

        // filter_callback
        Device& set_filter_callback(Callback& filter_cb) { 
            bool result;
            if(filter_first) result=filter_first->chain(filter_cb);
            else {
                filter_first = new Callback_Link(filter_cb);
                result = filter_first != NULL;
            }
            if(!result) {
                ulog(F("Trouble reserving filter entry."));
            }
            return *this;
        }
        Device& with_filter_callback(Callback& filter_cb) { 
            return set_filter_callback(filter_cb);
        }
        Device& filter_callback(Callback& filter_cb) { 
            return set_filter_callback(filter_cb);
        }
        Device& with_filter(Callback& filter_cb) { 
            return set_filter_callback(filter_cb);
        }
        Device& set_filter(Callback& filter_cb) { 
            return set_filter_callback(filter_cb);
        }
        Device& filter(Callback& filter_cb) { 
            return set_filter_callback(filter_cb);
        }
        bool call_filter_callbacks() {
            if(filter_first) {
                return filter_first->call(*this);
            } else {
                return true; // all ok if there are no filters
            }
        }

        // Subdevices
        Subdevice* add_subdevice(Subdevice* sd) {
            ulog(F("add_subdevice: device: %s subdev: >%s<"), name.as_cstr(), sd->get_name().as_cstr());
            if(subdevices.add(sd)) {
                ulog(F("add_subdevice >%s< succeeded."), sd->get_name().as_cstr());
                return sd;
            } else {
                ulog(F("add_subdevice >%s< failed."), sd->get_name().as_cstr());
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

        /* measure_init
         * Can be overloaded to allow restoring of some hardware
         * elements. For example a respective i2c bus due to a very weird
         * implementation of TwoWire in Wire.h
         * */
        virtual void measure_exit() {}; // usually do nothing here

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
// simple averaging filter
// average over buflen samples
class Filter_Average : public Callback {
    private:
        double sum=0;
        size_t values_count = 0;
        size_t _buflen = 0;
    public:
        Filter_Average(size_t buflen) : Callback() {
            _buflen = buflen;
        }
        bool call(Device &dev) {
            sum += dev.measured_value().as_float();
            values_count ++;
            if(values_count >= _buflen) {
                dev.measured_value().from(sum/values_count);
                values_count = 0;
                sum = 0;
                return true;
            }
            return false;
        }
};
#define filter_average(count) (*new Filter_Average(count))

// build an average over all samples arriving in a specific time window
class Filter_Time_Average : public Callback {
    private:
        double sum=0;
        size_t values_count = 0;
        uint32_t last_measured = millis();
        uint32_t _delta;
        double last_val = NAN;
    public:
        Filter_Time_Average(uint32_t delta) : Callback() {
            _delta=delta;
        }
        bool call(Device &dev) {
            sum += dev.measured_value().as_float();
            values_count ++;
            if(millis()-last_measured >= _delta) {
                last_measured = millis();
                double result;
                if(values_count == 0) { // no values read yet
                    if(last_val != NAN) {
                        result = last_val;
                    } else return false; // No value to return
                } else {
                    result = sum/values_count;
                }
                last_val = result;
                dev.measured_value().from(result);
                values_count = 0;
                sum = 0;
                return true;
            }
            return false;
        }
};
#define filter_time_average(delta_ms) (*new Filter_Time_Average(delta_ms))


// compute derivative in 1/ms
class Filter_Derivative : public Callback {
    private:
        double old_val = NAN;
        size_t values_count = 0;
        uint32_t old_time = micros();
    public:
        bool call(Device &dev) {
            uint32_t t = micros();
            bool retval = false;
            double v = dev.measured_value().as_float();
            if(old_val != NAN) {
                dev.measured_value().from((v - old_val) *1000 / (t-old_time));
                retval = true;
            }
            old_val = v;
            old_time = t;
            return retval;
        }
};
#define filter_derivative() (*new Filter_Derivative())


/* The Jeff McClintock running median estimate. 
 * base: https://stackoverflow.com/questions/10930732/c-efficiently-calculating-a-running-median
 * */
class Filter_JMC_Median : public Callback {
    private:
        double median = 0.0;
        double average = 0.0;
        uint32_t last_time = millis();
        uint32_t _update_ms;
    public:
        Filter_JMC_Median(uint32_t update_ms) : Callback() {
            _update_ms = update_ms;
        }
        bool call(Device &dev) {
            double sample = dev.measured_value().as_float();
            average += ( sample - average ) * 0.1;
            median += copysign( average * 0.01, sample - median );
            uint32_t current = millis() ;
            if(current - last_time >= _update_ms) {
                dev.measured_value().from(average);
                last_time = current;
                return true;
            }
            return false;
        }
};
#define filter_jmc_median(update_ms) (*new Filter_JMC_Median(update_ms))


/* Jmc median over small time intervals with reset after time runs out*/
#define filter_jmc_interval_median(interval, dev) \
    *new Callback([&](Device& dev) { \
        static double median; \
        static double average; \
        static bool undefined = true; \
        static unsigned long start_time; \
        if(undefined) { \
            start_time = millis(); \
            median = 0.0; \
            average = 0.0; \
            undefined = false; \
        } \
        double sample = dev.measured_value().as_float(); \
        average += ( sample - average ) * 0.1; \
        median += copysign( average * 0.01, sample - median ); \
        unsigned long current = millis() ; \
        if(current - start_time >= interval) { \
            dev.measured_value().from(average); \
            undefined = true; /* trigger reset */ \
            return true; \
        } \
        return false; \
    })


/* Turn analog values into binary with a threshold level */
#define filter_binarize(cutoff, high, low) \
    *new Callback([&](Device& dev) { \
        if(dev.measured_value().equals(low)) return false; \
        if(dev.measured_value().equals(high)) return false; \
        double sample = dev.measured_value().as_float(); \
        if(sample>=cutoff) { \
            dev.measured_value().from(high); \
        } else { \
            dev.measured_value().from(low); \
        } \
        return true; \
    })


/* round to the next multiple of base */
#define filter_round(base) \
    *new Callback( [&](Device& dev) { \
        int32_t v = (long)(dev.measured_value().as_float()/(base)+0.5); \
        dev.measured_value().from(v*(base)); \
        return true; \
    })


/* return maximum one value per time interval (interval in ms) */
#define filter_limit_time(interval, dev) \
    *new Callback( [&](Device& dev) { \
        static unsigned long last_time; \
        unsigned long current = millis() ; \
        if(!dev.measured_value().empty() \
           && current - last_time >= interval) { \
            last_time = current; \
            return true; \
        } \
        return false; \
    })


#endif // _IOTEMPOWER_DEVICE_H_
