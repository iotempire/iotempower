// filter.h
// author: ulno
// created: 2020-02-07
//
// Filters for IoTempower devices

#ifndef _IOTEMPOWER_FILTER_H_
#define _IOTEMPOWER_FILTER_H_

/* all included in device.h
#include <functional>
#include <toolbox.h>
#include <device.h>
*/

// general helper
#define filter(f) with_filter_callback(*new Callback(f))

// simple averaging filter
// average over buflen samples
class Filter_Average : public Callback {
    private:
        double sum=0;
        size_t values_count = 0;
        size_t _buflen;
    public:
        Filter_Average(size_t buflen) : Callback() {
            _buflen = buflen;
        }
        bool call(Device &dev) {
            sum += dev.read_float();
            values_count ++;
            if(values_count >= _buflen) {
                dev.write_float(sum/values_count);
                values_count = 0;
                sum = 0;
                return true;
            }
            return false;
        }
};
#define filter_average(count) with_filter_callback(*new Filter_Average(count))

// simple sliding averaging filter
// average over last buflen samples
class Filter_Sliding_Average : public Callback {
    private:
        double sum=0;
        size_t _buflen;
        size_t buffer_filled = 0;
        size_t buffer_position = 0;
        double *samples;
    public:
        Filter_Sliding_Average(size_t buflen) : Callback() {
            _buflen = buflen;
            samples = new double[_buflen];
            if(!samples) {
                ulog(F("Not enough memory for sliding average filter buffer."));
            }
        }
        bool call(Device &dev) {
            bool retval = false;
            if(samples) {
                double v = dev.read_float();
                sum += v; // add to overall sum
                buffer_filled ++;
                if(buffer_filled >= _buflen) { // only active when buffer filled
                    buffer_filled = _buflen; // keep at that size
                    // forget old value (that is on that new position) before overwriting
                    sum -= samples[buffer_position];
                    dev.write_float(sum/buffer_filled);
                    retval = true; // only return results when buffer filled
                }
                samples[buffer_position] = v; // store (overwrite when full) new value
                buffer_position = (buffer_position + 1) % _buflen; // move to next pos
            } // else not enough space, discard
            return retval; 
        }
};
#define filter_sliding_average(count) with_filter_callback(*new Filter_Sliding_Average(count))


// sliding minimum, maximum average filter
// average over last buflen samples
class Filter_Sliding_Min_Max_Avg : public Callback {
    private:
        double sum=0;
        size_t _buflen;
        size_t buffer_filled = 0;
        size_t buffer_position = 0;
        double *samples;
    public:
        Filter_Sliding_Min_Max_Avg(size_t buflen) : Callback() {
            _buflen = buflen;
            samples = new double[_buflen];
            if(!samples) {
                ulog(F("Not enough memory for sliding average filter buffer."));
            }
        }
        bool call(Device &dev) {
            bool retval = false;
            if(samples) {
                double v = dev.read_float();
                sum += v; // add to overall sum
                buffer_filled ++;
                if(buffer_filled >= _buflen) { // only active when buffer filled
                    buffer_filled = _buflen; // keep at that size
                    // forget old value (that is on that new position) before overwriting
                    sum -= samples[buffer_position];
                }
                samples[buffer_position] = v; // store (overwrite when full) new value
                buffer_position = (buffer_position + 1) % _buflen; // move to next pos
                if(buffer_filled >= _buflen) {
                    double _min = samples[0], _max=samples[0];
                    for(int i=1; i<buffer_filled; i++) {
                        double s = samples[i];
                        if(s < _min) _min=s;
                        if(s > _max) _max=s;
                    }
                    dev.write_float((_min + _max) / 2);
                    retval = true; // only return results when buffer filled
                }
            } // else not enough space, discard
            return retval; 
        }
};
#define filter_sliding_min_max_avg(count) with_filter_callback(*new Filter_Sliding_Min_Max_Avg(count))


// sliding maximum filter
// average over last buflen samples
class Filter_Sliding_Max : public Callback {
    private:
        double sum=0;
        size_t _buflen;
        size_t buffer_filled = 0;
        size_t buffer_position = 0;
        double *samples;
    public:
        Filter_Sliding_Max(size_t buflen) : Callback() {
            _buflen = buflen;
            samples = new double[_buflen];
            if(!samples) {
                ulog(F("Not enough memory for sliding average filter buffer."));
            }
        }
        bool call(Device &dev) {
            bool retval = false;
            if(samples) {
                double v = dev.read_float();
                sum += v; // add to overall sum
                buffer_filled ++;
                if(buffer_filled >= _buflen) { // only active when buffer filled
                    buffer_filled = _buflen; // keep at that size
                    // forget old value (that is on that new position) before overwriting
                    sum -= samples[buffer_position];
                }
                samples[buffer_position] = v; // store (overwrite when full) new value
                buffer_position = (buffer_position + 1) % _buflen; // move to next pos
                if(buffer_filled >= _buflen) {
                    double _max=samples[0];
                    for(int i=1; i<buffer_filled; i++) {
                        double s = samples[i];
                        if(s > _max) _max=s;
                    }
                    dev.write_float(_max);
                    retval = true; // only return results when buffer filled
                }
            } // else not enough space, discard
            return retval; 
        }
};
#define filter_sliding_max(count) with_filter_callback(*new Filter_Sliding_Max(count))

// sliding maximum filter
// average over last buflen samples
class Filter_Sliding_Min : public Callback {
    private:
        double sum=0;
        size_t _buflen;
        size_t buffer_filled = 0;
        size_t buffer_position = 0;
        double *samples;
    public:
        Filter_Sliding_Min(size_t buflen) : Callback() {
            _buflen = buflen;
            samples = new double[_buflen];
            if(!samples) {
                ulog(F("Not enough memory for sliding average filter buffer."));
            }
        }
        bool call(Device &dev) {
            bool retval = false;
            if(samples) {
                double v = dev.read_float();
                sum += v; // add to overall sum
                buffer_filled ++;
                if(buffer_filled >= _buflen) { // only active when buffer filled
                    buffer_filled = _buflen; // keep at that size
                    // forget old value (that is on that new position) before overwriting
                    sum -= samples[buffer_position];
                }
                samples[buffer_position] = v; // store (overwrite when full) new value
                buffer_position = (buffer_position + 1) % _buflen; // move to next pos
                if(buffer_filled >= _buflen) {
                    double _min=samples[0];
                    for(int i=1; i<buffer_filled; i++) {
                        double s = samples[i];
                        if(s < _min) _min=s;
                    }
                    dev.write_float(_min);
                    retval = true; // only return results when buffer filled
                }
            } // else not enough space, discard
            return retval; 
        }
};
#define filter_sliding_min(count) with_filter_callback(*new Filter_Sliding_Min(count))


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
            sum += dev.read_float();
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
                dev.write_float(result);
                values_count = 0;
                sum = 0;
                return true;
            }
            return false;
        }
};
#define filter_time_average(delta_ms) with_filter_callback(*new Filter_Time_Average(delta_ms))


// moving median filter over last buflen samples
class Filter_Moving_Median : public Callback {
    private:
        size_t _buflen;
        double *samples;
        size_t samples_filled = 0;
        size_t samples_position = 0;
        double *sorted;
    public:
        Filter_Moving_Median(size_t buflen) : Callback() {
            _buflen = buflen;
            samples = new double[_buflen];
            sorted = new double[_buflen];
            if(!samples || !sorted) {
                samples = NULL;
                ulog(F("Not enough memory for moving median filter buffers."));
            }
        }
        bool call(Device &dev) {
            bool retval = false;
            if(samples) {
                double v = dev.read_float();
                double oldv;
                samples_filled ++;
                if(samples_filled >= _buflen) { // only active when buffer filled
                    samples_filled = _buflen; // keep at that size
                    // remember old value
                    oldv = samples[samples_position];
                    samples[samples_position] = v; // overwrite with new value
                    // remove old value from sorted and add new one
                    bool removed_old = false;
                    bool inserted_new = false;
                    for(int pos=0; pos<samples_filled-1; pos++)
                    {
                        if(!removed_old) {
                            if(sorted[pos] == oldv) {
                                removed_old = true; // from now on copy higher values to current pos;
                            }
                        }
                        if(removed_old) {
                            sorted[pos] = sorted[pos+1];
                        }
                        if(v <= sorted[pos]) { // needs to be inserted here
                            double tmp = sorted[pos];
                            sorted[pos] = v;
                            v = tmp; // remember to insert in next round
                            inserted_new = true;
                            if(removed_old) {
                                break; // found and replaced
                            }
                        }
                    }
                    if(!inserted_new) { // if it hasn't been stored already
                        sorted[samples_filled-1] = v;
                    }
                    dev.write_float(sorted[samples_filled/2]);
                    retval = true; // only return results when buffer filled
                } else { // just add to buffers
                    samples[samples_position] = v; // store new value
                    // add sorted
                    for(int pos=0; pos<samples_filled-1; pos++)
                    {
                        if(v <= sorted[pos]) { // needs to be inserted here
                            double tmp = sorted[pos];
                            sorted[pos] = v;
                            v = tmp; // remember to insert in next round
                        }
                    }
                }

                // // debug, TODO: remove 
                // for(int pos=0; pos<samples_filled-1; pos++) {
                //     Serial.print(sorted[pos]);
                //     Serial.print(" ");
                // }
                // Serial.println();

                samples_position = (samples_position + 1) % _buflen; // move to next pos
            } // else not enough space, discard
            return retval; 
        }
};
#define filter_moving_median(count) with_filter_callback(*new Filter_Moving_Median(count))



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
            double v = dev.read_float();
            if(old_val != NAN) {
                dev.write_float((v - old_val) *1000 / (t-old_time));
                retval = true;
            }
            old_val = v;
            old_time = t;
            return retval;
        }
};
#define filter_derivative() with_filter_callback(*new Filter_Derivative())


// only report value if a minimum change to last value happened
// (often called precision)
template<class TYPE>
class Filter_Minchange : public Callback {
    private:
        TYPE old_val = 0;
        TYPE _min_change;
    public:
        Filter_Minchange(uint32_t min_change) : Callback() {
            _min_change=min_change;
        }
        bool call(Device &dev) {
            TYPE v;
            if(std::is_same<TYPE, double>::value) {
                v = dev.read_float();
            } else {
                v = dev.read_int();
            }
            if(abs(old_val - v) >= _min_change) {
                dev.value().from(v);
                old_val = v;
                return true;
            }
            return false;
        }
};
#define filter_minchange(minchange) with_filter_callback(*new Filter_Minchange<double>(minchange))


/* The Jeff McClintock running median estimate. 
 * base: https://stackoverflow.com/questions/10930732/c-efficiently-calculating-a-running-median
 * */
class Filter_JMC_Median : public Callback {
    private:
        double median = 0.0;
        double average = 0.0;
    public:
        bool call(Device &dev) {
            double sample = dev.read_float();
            average += (sample - average) * 0.1;
            median += copysign(average * 0.01, sample - median);
            dev.write_float(average);
            return true;
        }
};
#define filter_jmc_median() with_filter_callback(*new Filter_JMC_Median())

/* Jmc median over small time intervals with reset after time runs out*/
// TODO rewrite as class to avoid local closure issues
#define filter_jmc_interval_median(interval) with_filter_callback(\
    *new Callback([](Device& dev) { \
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
        double sample = dev.read_float(); \
        average += ( sample - average ) * 0.1; \
        median += copysign( average * 0.01, sample - median ); \
        unsigned long current = millis() ; \
        if(current - start_time >= interval) { \
            dev.write_float(average); \
            undefined = true; /* trigger reset */ \
            return true; \
        } \
        return false; \
    }))


/* Filter restricting values to an interval (forget all outside interval) */
class Filter_Restrict : public Callback {
    private:
        double _from = 0.0;
        double _to = 0.0;
    public:
        Filter_Restrict(double from, double to) : Callback() {
            _from = from;
            _to = to;
        }
        bool call(Device &dev) {
            double sample = dev.read_float();
            if(sample < _from || sample > _to) return false;
            return true;
        }
};
#define filter_restrict(from, to) with_filter_callback(*new Filter_Restrict(from, to))


/* round to the next multiple of base */
// TODO rewrite as class to avoid local closure issues
#define filter_round(base) with_filter_callback(\
    *new Callback( [](Device& dev) { \
        int32_t v = (long)(dev.read_float()/(base)+0.5); \
        dev.write_int(v*(base)); \
        return true; \
    }))


/* return maximum one value per time interval (interval in ms) */
// TODO rewrite as class to avoid local closure issues
#define filter_limit_time(interval) with_filter_callback(\
    *new Callback( [](Device& dev) { \
        static unsigned long last_time; \
        unsigned long current = millis() ; \
        if(!dev.value().empty() \
           && current - last_time >= interval) { \
            last_time = current; \
            return true; \
        } \
        return false; \
    }))

// A filter that can detect, clicks, double clicks, long clicks and too long clicks
class Filter_Click_Detector : public Callback {
    private:
        size_t values_count = 0;
        uint32_t event_times[4];
        const char* none = "released";
        const char* click = "click";
        const char* double_click = "double";
        const char* long_click = "long";
        uint32_t _click_min_ms;
        uint32_t _click_max_ms;
        uint32_t _longclick_min_ms;
        uint32_t _longclick_max_ms;
        uint32_t _double_distance_ms;
        bool last_was_released = true;
        const char *_released;
        const char *_pressed;
        bool _early_click;
        bool click_registered = false;
    public:
        Filter_Click_Detector(bool early_click = false, 
            uint32_t click_min_ms=20, uint32_t click_max_ms=500,
            uint32_t longclick_min_ms=1000, uint32_t longclick_max_ms=2500,
            const char* pressed=str_on, const char* released=str_off) : Callback() {

            _click_min_ms = click_min_ms;
            _click_max_ms = click_max_ms;
            _longclick_min_ms = longclick_min_ms;
            _longclick_max_ms = longclick_max_ms;
            _double_distance_ms = _click_max_ms/4;
            _early_click = early_click;
            _released = released;
            _pressed = pressed;
            for(int i=0; i<4; i++) {
                event_times[i] = millis() - _longclick_max_ms; // make them long past in history
            }
        }
        bool call(Device &dev) {
            bool is_released = dev.is(_released);
            if(is_released || dev.is(_pressed)) { // is pressed or released?
                // only store when changed
                if(is_released != last_was_released) {
                    last_was_released = is_released;
                    // move events down
                    for(int i=0; i<3; i++) {
                        event_times[i] = event_times[i+1];
                    }
                    event_times[3] = millis();
                    if(_early_click) {
                        if(!is_released) { // if (just) pressed
                            // analyse what type of click this is
                            // max longclick not evaluated
                            // long_click is handled in keeping things down
                            // check if normal or double
                            if(event_times[3] - event_times[1] 
                                >= _click_max_ms + _double_distance_ms) {
                                // if distance is too big-> normal click
                                // check if it was a double
                                dev.write(click);
                            } else { // must have been a double 
                                dev.write(double_click);
                            }
                            return true;
                        } // endif is_released
                    } else { // not early_click
                        if(is_released) {
                            // analyse what type of click this is
                            if(event_times[3] - event_times[2] >= _longclick_max_ms) {
                                dev.write(none); // was pressed too long
                                return true;
                            }
                            if(event_times[3] -  event_times[2] >= _longclick_min_ms) {
                                dev.write(long_click); // was in long click interval
                                click_registered = false;
                                return true;
                            }
                            if(event_times[3] -  event_times[2] >= _click_max_ms) {
                                // was pressed too long for normal click but too short for long click
                                dev.write(none);
                                return true;
                            }
                            if(event_times[3] -  event_times[2] >= _click_min_ms) {
                                // this is a normal click
                                // check if it was a double
                                if(event_times[3] - event_times[0] >= 2*_click_max_ms
                                    || event_times[2] - event_times[1] > _double_distance_ms) {
                                    click_registered = true;
                                    return false;
                                } else {
                                    dev.write(double_click);
                                }
                                click_registered = false;
                                return true;
                            }
                        } // endif is_released
                    } // endif early_click
                } else { // Same value as before
                    if(_early_click) {
                        if(!is_released) { // still pressed
                            if(millis() -  event_times[3] >= _longclick_min_ms) {
                                dev.write(long_click);
                                return true;
                            } // TODO: fix multiple long clicks when held
                        } else {
                            // make sure to notify that now things are released
                            dev.write(none);
                            return true;
                        }
                    } else { // if not early_click
                        if(is_released) {
                            if(click_registered && // if click happened, check it was no double click
                                (millis()-event_times[3]) > _double_distance_ms) {

                                dev.write(click);
                                click_registered = false;
                            } else {
                                // make sure to notify that now things are released
                                dev.write(none);
                            }
                            return true;
                        }
                    }
                }
            } // neither released nor pressed
            return false;
        }
};
#define filter_click_detector(...) \
    with_filter_callback(*new Filter_Click_Detector(false, __VA_ARGS__))
#define filter_early_click_detector(...) \
    with_filter_callback(*new Filter_Click_Detector(true, __VA_ARGS__))


/* Turn analog values into binary with a threshold level */
// #define filter_binarize(cutoff, high, low) with_filter_callback( \
    // *new Callback([](Device& dev) { \
        // if(dev.value().equals(low)) return false; \
        // if(dev.value().equals(high)) return false; \
        // double sample = dev.value().as_float(); \
        // if(sample>=cutoff) { \
            // dev.value().from(high); \
        // } else { \
            // dev.value().from(low); \
        // } \
        // return true; \
    // }))
class Filter_Binarize : public Callback {
    private:
        double _cutoff;
        const char* _high;
        const char* _low;
    public:
        Filter_Binarize(double cutoff, const char* high, const char* low) : Callback() {
            _cutoff = cutoff;
            _high = high;
            _low = low;
        }
        bool call(Device &dev) {
            if(dev.value().equals(_low)) return false;
            if(dev.value().equals(_high)) return false;
            double sample = dev.value().as_float();
            if(sample >= _cutoff) {
                dev.value().from(_high);
            } else {
                dev.value().from(_low);
            }
            return true;
        }
};
#define filter_binarize(cutoff, high, low) with_filter_callback(*new Filter_Binarize(cutoff, high, low))


/* map set of intervals to single descrite values
 * always assumed to start at minus infinity and needs is automatically terminated by infinity
 * example parameters: "low", -0.5, NULL, 0.5, "high"
 * NULL means the inerval is skipped
 * */
class Filter_Interval_Map : public Callback {
    private:
        double *borders;
        const char* *values;
        int count = 0;
        bool _allocate(int c) {
            borders =  new double[c];
            values = new const char*[c];
            if(borders && values) {
                return true;
            }
            ulog(F("Trouble allocating %d elements for interval map filter."), c);
            return false;
        }
        void _add(const char* v, double b) {
            values[count] = v;
            borders[count] = b;
            count ++;
        }
    public:
        Filter_Interval_Map(const char* v0) : Callback() {
            if(_allocate(1)) {
                _add(v0, INFINITY);            
            }
        }
        Filter_Interval_Map(const char* v0, double b0, 
                            const char* v1) : Callback() {
            if(_allocate(2)) {
                _add(v0, b0);            
                _add(v1, INFINITY);            
            }
        }
        Filter_Interval_Map(const char* v0, double b0, 
                            const char* v1, double b1,
                            const char* v2) : Callback() {
            if(_allocate(3)) {
                _add(v0, b0);            
                _add(v1, b1);            
                _add(v2, INFINITY);            
            }
        }
        Filter_Interval_Map(const char* v0, double b0, 
                            const char* v1, double b1,
                            const char* v2, double b2,
                            const char* v3) : Callback() {
            if(_allocate(4)) {
                _add(v0, b0);
                _add(v1, b1);
                _add(v2, b2);
                _add(v3, INFINITY);
            }
        }
        Filter_Interval_Map(const char* v0, double b0, 
                            const char* v1, double b1,
                            const char* v2, double b2,
                            const char* v3, double b3,
                            const char* v4) : Callback() {
            if(_allocate(5)) {
                _add(v0, b0);
                _add(v1, b1);
                _add(v2, b2);
                _add(v3, b3);
                _add(v4, INFINITY);
            }
        }
        // TODO allow more values

        bool call(Device &dev) {
            double v = dev.read_float();
            for(int i=0; i<=count; i++) { // make infinity as values included
                if(v<borders[i]) {
                    if(values[i]) {
                        dev.write(values[i]);
                        return true;
                    } else return false; // when NULL discard value
                }
            }
            return false;
        }
};
#define filter_interval_map(...) with_filter_callback(*new Filter_Interval_Map(__VA_ARGS__))


#endif // _IOTEMPOWER_FILTER_H_
