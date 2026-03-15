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

// Include auto-generated markers (if they exist)
#if __has_include("markers_generated.h")
    #include "markers_generated.h"
#endif

// Macro to access markers
#define MARKED(marker_name) iotempower_marker_##marker_name

// general helper
#define filter(f) with_filter_callback(*new Callback(f))

// Marker support classes
class Filter_CSV_Mark : public Callback {
    private:
        Ustring* _target;
        int _value_index;
        int _csv_field_index;
        
    public:
        Filter_CSV_Mark(Ustring* target, int field_index, int value_index = 0) : Callback() {
            _target = target;
            _value_index = value_index;
            _csv_field_index = field_index;
        }
        
        bool call(Device &dev) {
            // Parse CSV field using Ustring class
            Ustring csv_data;
            csv_data.from(dev.value(_value_index).as_cstr());
            
            // Extract field using simple parsing
            int current_field = 0;
            int field_start = 0;
            int pos = 0;
            
            while(pos < csv_data.length() && current_field < _csv_field_index) {
                if(csv_data.as_cstr()[pos] == ',') {
                    current_field++;
                    field_start = pos + 1;
                }
                pos++;
            }
            
            if(current_field == _csv_field_index) {
                // Find end of field
                int field_end = field_start;
                while(field_end < csv_data.length() && csv_data.as_cstr()[field_end] != ',') {
                    field_end++;
                }
                
                // Extract field value directly into target Ustring
                _target->clear();
                _target->copy(csv_data, field_start, field_end - field_start);
            }
            return true; // Forward the now marked and copied value in the filter chain
        }
};

///// Marker macros
// Mark a CSV field from value_index into marker
#define mark_csv_field(marker_name, field_index, value_index) \
    with_filter_callback(*new Filter_CSV_Mark(&iotempower_marker_##marker_name, field_index, value_index))

// Mark current value into marker
#define mark_field(marker_name, value_index) \
    with_filter_callback(*new Callback([](Device& dev) { \
        MARKED(marker_name).copy(dev.value(value_index)); \
        return true; \
    }))

/**
 * Generic helper macro to dispatch function-like macros based on argument count.
 * This macro examines the number of arguments passed and selects the appropriate
 * implementation macro to call. It's used to create overloaded macro behavior
 * similar to function overloading in C++.
 * 
 * The macro works by:
 * 1. Adding dummy arguments to fill positions
 * 2. Using argument position to select the correct implementation
 * 3. The NAME parameter ends up being the selected macro to call
 * 4. This allows 1-argument vs 2-argument macro dispatch
 */
#define IOTEMPOWER_DISPATCH_2(_1, _2, NAME, ...) NAME
#define IOTEMPOWER_DISPATCH_3(_1, _2, _3, NAME, ...) NAME
#define IOTEMPOWER_DISPATCH_4(_1, _2, _3, _4, NAME, ...) NAME

// Commit marker macro with optional clear flag
#define commit_mark(...) \
    IOTEMPOWER_DISPATCH_3(__VA_ARGS__, \
                     commit_mark_with_clear_, \
                     commit_mark_no_clear_, \
                     commit_mark_no_clear_)(__VA_ARGS__)

// Implementation without third parameter (no clear flag)
#define commit_mark_no_clear_(marker_name, value_index, ...) \
    with_filter_callback(*new Callback([](Device& dev) { \
        dev.value(value_index).copy(MARKED(marker_name)); \
        return true; \
    }))

// Implementation with third parameter (boolean clear flag)
#define commit_mark_with_clear_(marker_name, value_index, clear_flag) \
    with_filter_callback(*new Callback([](Device& dev) { \
        if (clear_flag) dev.clear_all_values(); \
        dev.value(value_index).copy(MARKED(marker_name)); \
        return true; \
    }))

// Commit CSV marks - writes multiple markers as comma-separated values
// Usage: commit_csv_marks(value_index, mark1, mark2, mark3, ...)

// Helper macro to apply MARKED() to each argument - needs indirection for proper expansion
#define IOTEMPOWER_APPLY_MARKED(x) &MARKED(x)
#define IOTEMPOWER_MAP_1(f, x) f(x)
#define IOTEMPOWER_MAP_2(f, x, ...) f(x), IOTEMPOWER_MAP_1(f, __VA_ARGS__)
#define IOTEMPOWER_MAP_3(f, x, ...) f(x), IOTEMPOWER_MAP_2(f, __VA_ARGS__)
#define IOTEMPOWER_MAP_4(f, x, ...) f(x), IOTEMPOWER_MAP_3(f, __VA_ARGS__)
#define IOTEMPOWER_MAP_5(f, x, ...) f(x), IOTEMPOWER_MAP_4(f, __VA_ARGS__)
#define IOTEMPOWER_MAP_6(f, x, ...) f(x), IOTEMPOWER_MAP_5(f, __VA_ARGS__)
#define IOTEMPOWER_MAP_7(f, x, ...) f(x), IOTEMPOWER_MAP_6(f, __VA_ARGS__)
#define IOTEMPOWER_MAP_8(f, x, ...) f(x), IOTEMPOWER_MAP_7(f, __VA_ARGS__)
#define IOTEMPOWER_MAP_9(f, x, ...) f(x), IOTEMPOWER_MAP_8(f, __VA_ARGS__)
#define IOTEMPOWER_MAP_10(f, x, ...) f(x), IOTEMPOWER_MAP_9(f, __VA_ARGS__)

// Count and dispatch macros - need extra indirection for proper expansion
#define IOTEMPOWER_NARG_HELPER_(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define IOTEMPOWER_NARG_(...) IOTEMPOWER_NARG_HELPER_(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define IOTEMPOWER_NARG(...) IOTEMPOWER_NARG_(__VA_ARGS__)
#define IOTEMPOWER_MAP__(N, f, ...) IOTEMPOWER_MAP_##N(f, __VA_ARGS__)
#define IOTEMPOWER_MAP_(N, f, ...) IOTEMPOWER_MAP__(N, f, __VA_ARGS__)
#define IOTEMPOWER_MAP(f, ...) IOTEMPOWER_MAP_(IOTEMPOWER_NARG(__VA_ARGS__), f, __VA_ARGS__)

// Main macro - simpler approach using direct lambda
#define commit_csv_marks(value_index, ...) \
    with_filter_callback(*new Callback([](Device& dev) { \
        Ustring* markers[] = {IOTEMPOWER_MAP(IOTEMPOWER_APPLY_MARKED, __VA_ARGS__)}; \
        int count = sizeof(markers) / sizeof(markers[0]); \
        if (count > 0) { \
            dev.value(value_index).copy(*markers[0]); \
            for (int i = 1; i < count; i++) { \
                dev.value(value_index).add(","); \
                dev.value(value_index).add(*markers[i]); \
            } \
        } \
        return true; \
    }))

// Clear value helper - clears a specific value index
#define clear_value(value_index) \
    with_filter_callback(*new Callback([](Device& dev) { \
        dev.value(value_index).clear(); \
        return true; \
    }))

// simple averaging filter
// average over buflen samples
class Filter_Average : public Callback {
    private:
        double sum=0;
        size_t values_count = 0;
        size_t _buflen;
        Ustring& _target;
    public:
        Filter_Average(size_t buflen) : Callback(), _target(get_device().value()) {
            _buflen = buflen;
        }
        Filter_Average(Ustring& target, size_t buflen) : Callback(), _target(target) {
            _buflen = buflen;
        }
        bool call(Device &dev) {
            sum += _target.as_float();
            values_count ++;
            if(values_count >= _buflen) {
                _target.from(sum/values_count);
                values_count = 0;
                sum = 0;
                return true;
            }
            return false;
        }
};
// Filter average macro with automatic marker expansion
#define filter_average(...) \
    IOTEMPOWER_DISPATCH_2(__VA_ARGS__, \
                     filter_average_with_marker_, \
                     filter_average_simple_)(__VA_ARGS__)

// Implementation with one parameter (just buffer size)
#define filter_average_simple_(buflen) \
    with_filter_callback(*new Filter_Average(buflen))

// Implementation with two parameters (marker name + buffer size)
#define filter_average_with_marker_(marker_name, buflen) \
    with_filter_callback(*new Filter_Average(MARKED(marker_name), buflen))


// simple sliding averaging filter
// average over last buflen samples
class Filter_Sliding_Average : public Callback {
    private:
        double sum=0;
        size_t _buflen;
        size_t buffer_filled = 0;
        size_t buffer_position = 0;
        double *samples;
        Ustring& _target;
    public:
        Filter_Sliding_Average(size_t buflen) : Callback(), _target(get_device().value()) {
            _buflen = buflen;
            samples = new double[_buflen];
            if(!samples) {
                ulog(F("Not enough memory for sliding average filter buffer."));
            }
        }
        Filter_Sliding_Average(Ustring& target, size_t buflen) : Callback(), _target(target) {
            _buflen = buflen;
            samples = new double[_buflen];
            if(!samples) {
                ulog(F("Not enough memory for sliding average filter buffer."));
            }
        }
        bool call(Device &dev) {
            bool retval = false;
            if(samples) {
                double v = _target.as_float();
                sum += v; // add to overall sum
                buffer_filled ++;
                if(buffer_filled >= _buflen) { // only active when buffer filled
                    buffer_filled = _buflen; // keep at that size
                    // forget old value (that is on that new position) before overwriting
                    sum -= samples[buffer_position];
                    _target.from(sum/buffer_filled);
                    retval = true; // only return results when buffer filled
                }
                samples[buffer_position] = v; // store (overwrite when full) new value
                buffer_position = (buffer_position + 1) % _buflen; // move to next pos
            } // else not enough space, discard
            return retval; 
        }
};
// Filter sliding average macro with automatic marker expansion
#define filter_sliding_average(...) \
    IOTEMPOWER_DISPATCH_2(__VA_ARGS__, \
                     filter_sliding_average_with_marker_, \
                     filter_sliding_average_simple_)(__VA_ARGS__)

// Implementation with one parameter (just buffer size)
#define filter_sliding_average_simple_(buflen) \
    with_filter_callback(*new Filter_Sliding_Average(buflen))

// Implementation with two parameters (marker name + buffer size)
#define filter_sliding_average_with_marker_(marker_name, buflen) \
    with_filter_callback(*new Filter_Sliding_Average(MARKED(marker_name), buflen))


// sliding minimum, maximum average filter
// average over last buflen samples
class Filter_Sliding_Min_Max_Avg : public Callback {
    private:
        double sum=0;
        size_t _buflen;
        size_t buffer_filled = 0;
        size_t buffer_position = 0;
        double *samples;
        Ustring& _target;
    public:
        Filter_Sliding_Min_Max_Avg(size_t buflen) : Callback(), _target(get_device().value()) {
            _buflen = buflen;
            samples = new double[_buflen];
            if(!samples) {
                ulog(F("Not enough memory for sliding average filter buffer."));
            }
        }
        Filter_Sliding_Min_Max_Avg(Ustring& target, size_t buflen) : Callback(), _target(target) {
            _buflen = buflen;
            samples = new double[_buflen];
            if(!samples) {
                ulog(F("Not enough memory for sliding average filter buffer."));
            }
        }
        bool call(Device &dev) {
            bool retval = false;
            if(samples) {
                double v = _target.as_float();
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
                    _target.from((_min + _max) / 2);
                    retval = true; // only return results when buffer filled
                }
            } // else not enough space, discard
            return retval; 
        }
};
// Filter sliding min max avg macro with automatic marker expansion
#define filter_sliding_min_max_avg(...) \
    IOTEMPOWER_DISPATCH_2(__VA_ARGS__, \
                     filter_sliding_min_max_avg_with_marker_, \
                     filter_sliding_min_max_avg_simple_)(__VA_ARGS__)

// Implementation with one parameter (just buffer size)
#define filter_sliding_min_max_avg_simple_(buflen) \
    with_filter_callback(*new Filter_Sliding_Min_Max_Avg(buflen))

// Implementation with two parameters (marker name + buffer size)
#define filter_sliding_min_max_avg_with_marker_(marker_name, buflen) \
    with_filter_callback(*new Filter_Sliding_Min_Max_Avg(MARKED(marker_name), buflen))


// sliding maximum filter
// average over last buflen samples
class Filter_Sliding_Max : public Callback {
    private:
        double sum=0;
        size_t _buflen;
        size_t buffer_filled = 0;
        size_t buffer_position = 0;
        double *samples;
        Ustring& _target;
    public:
        Filter_Sliding_Max(size_t buflen) : Callback(), _target(get_device().value()) {
            _buflen = buflen;
            samples = new double[_buflen];
            if(!samples) {
                ulog(F("Not enough memory for sliding average filter buffer."));
            }
        }
        Filter_Sliding_Max(Ustring& target, size_t buflen) : Callback(), _target(target) {
            _buflen = buflen;
            samples = new double[_buflen];
            if(!samples) {
                ulog(F("Not enough memory for sliding average filter buffer."));
            }
        }
        bool call(Device &dev) {
            bool retval = false;
            if(samples) {
                double v = _target.as_float();
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
                    _target.from(_max);
                    retval = true; // only return results when buffer filled
                }
            } // else not enough space, discard
            return retval; 
        }
};
// Filter sliding max macro with automatic marker expansion
#define filter_sliding_max(...) \
    IOTEMPOWER_DISPATCH_2(__VA_ARGS__, \
                     filter_sliding_max_with_marker_, \
                     filter_sliding_max_simple_)(__VA_ARGS__)

// Implementation with one parameter (just buffer size)
#define filter_sliding_max_simple_(buflen) \
    with_filter_callback(*new Filter_Sliding_Max(buflen))

// Implementation with two parameters (marker name + buffer size)
#define filter_sliding_max_with_marker_(marker_name, buflen) \
    with_filter_callback(*new Filter_Sliding_Max(MARKED(marker_name), buflen))

// sliding minimum filter
// average over last buflen samples
class Filter_Sliding_Min : public Callback {
    private:
        double sum=0;
        size_t _buflen;
        size_t buffer_filled = 0;
        size_t buffer_position = 0;
        double *samples;
        Ustring& _target;
    public:
        Filter_Sliding_Min(size_t buflen) : Callback(), _target(get_device().value()) {
            _buflen = buflen;
            samples = new double[_buflen];
            if(!samples) {
                ulog(F("Not enough memory for sliding average filter buffer."));
            }
        }
        Filter_Sliding_Min(Ustring& target, size_t buflen) : Callback(), _target(target) {
            _buflen = buflen;
            samples = new double[_buflen];
            if(!samples) {
                ulog(F("Not enough memory for sliding average filter buffer."));
            }
        }
        bool call(Device &dev) {
            bool retval = false;
            if(samples) {
                double v = _target.as_float();
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
                    _target.from(_min);
                    retval = true; // only return results when buffer filled
                }
            } // else not enough space, discard
            return retval; 
        }
};
// Filter sliding min macro with automatic marker expansion
#define filter_sliding_min(...) \
    IOTEMPOWER_DISPATCH_2(__VA_ARGS__, \
                     filter_sliding_min_with_marker_, \
                     filter_sliding_min_simple_)(__VA_ARGS__)

// Implementation with one parameter (just buffer size)
#define filter_sliding_min_simple_(buflen) \
    with_filter_callback(*new Filter_Sliding_Min(buflen))

// Implementation with two parameters (marker name + buffer size)
#define filter_sliding_min_with_marker_(marker_name, buflen) \
    with_filter_callback(*new Filter_Sliding_Min(MARKED(marker_name), buflen))


// build an average over all samples arriving in a specific time window
class Filter_Time_Average : public Callback {
    private:
        double sum=0;
        size_t values_count = 0;
        uint32_t last_measured = millis();
        uint32_t _delta;
        double last_val = NAN;
        Ustring& _target;
    public:
        Filter_Time_Average(uint32_t delta) : Callback(), _target(get_device().value()) {
            _delta=delta;
        }
        Filter_Time_Average(Ustring& target, uint32_t delta) : Callback(), _target(target) {
            _delta=delta;
        }
        bool call(Device &dev) {
            sum += _target.as_float();
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
                _target.from(result);
                values_count = 0;
                sum = 0;
                return true;
            }
            return false;
        }
};
// Filter time average macro with automatic marker expansion
#define filter_time_average(...) \
    IOTEMPOWER_DISPATCH_2(__VA_ARGS__, \
                     filter_time_average_with_marker_, \
                     filter_time_average_simple_)(__VA_ARGS__)

// Implementation with one parameter (just delta)
#define filter_time_average_simple_(delta_ms) \
    with_filter_callback(*new Filter_Time_Average(delta_ms))

// Implementation with two parameters (marker name + delta)
#define filter_time_average_with_marker_(marker_name, delta_ms) \
    with_filter_callback(*new Filter_Time_Average(MARKED(marker_name), delta_ms))


// moving median filter over last buflen samples
class Filter_Moving_Median : public Callback {
    private:
        size_t _buflen;
        double *samples;
        size_t samples_filled = 0;
        size_t samples_position = 0;
        double *sorted;
        Ustring& _target;
    public:
        Filter_Moving_Median(size_t buflen) : Callback(), _target(get_device().value()) {
            _buflen = buflen;
            samples = new double[_buflen];
            sorted = new double[_buflen];
            if(!samples || !sorted) {
                samples = NULL;
                ulog(F("Not enough memory for moving median filter buffers."));
            }
        }
        Filter_Moving_Median(Ustring& target, size_t buflen) : Callback(), _target(target) {
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
                double v = _target.as_float();
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
                    _target.from(sorted[samples_filled/2]);
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
// Filter moving median macro with automatic marker expansion
#define filter_moving_median(...) \
    IOTEMPOWER_DISPATCH_2(__VA_ARGS__, \
                     filter_moving_median_with_marker_, \
                     filter_moving_median_simple_)(__VA_ARGS__)

// Implementation with one parameter (just buffer size)
#define filter_moving_median_simple_(buflen) \
    with_filter_callback(*new Filter_Moving_Median(buflen))

// Implementation with two parameters (marker name + buffer size)
#define filter_moving_median_with_marker_(marker_name, buflen) \
    with_filter_callback(*new Filter_Moving_Median(MARKED(marker_name), buflen))



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
        Ustring& _target;
    public:
        Filter_Minchange(uint32_t min_change) : Callback(), _target(get_device().value()) {
            _min_change=min_change;
        }
        Filter_Minchange(Ustring& target, uint32_t min_change) : Callback(), _target(target) {
            _min_change=min_change;
        }
        bool call(Device &dev) {
            TYPE v;
            if(std::is_same<TYPE, double>::value) {
                v = _target.as_float();
            } else {
                v = _target.as_int();
            }
            if(abs(old_val - v) >= _min_change) {
                _target.from(v);
                old_val = v;
                return true;
            }
            return false;
        }
};
// Filter minchange macro with automatic marker expansion
#define filter_minchange(...) \
    IOTEMPOWER_DISPATCH_2(__VA_ARGS__, \
                     filter_minchange_with_marker_, \
                     filter_minchange_simple_)(__VA_ARGS__)

// Implementation with one parameter (just min change)
#define filter_minchange_simple_(minchange) \
    with_filter_callback(*new Filter_Minchange<double>(minchange))

// Implementation with two parameters (marker name + min change)
#define filter_minchange_with_marker_(marker_name, minchange) \
    with_filter_callback(*new Filter_Minchange<double>(MARKED(marker_name), minchange))


/* The Jeff McClintock running median estimate. 
 * base: https://stackoverflow.com/questions/10930732/c-efficiently-calculating-a-running-median
 * */
class Filter_JMC_Median : public Callback {
    private:
        double median = 0.0;
        double average = 0.0;
        Ustring& _target;
    public:
        Filter_JMC_Median() : Callback(), _target(get_device().value()) {
        }
        Filter_JMC_Median(Ustring& target) : Callback(), _target(target) {
        }
        bool call(Device &dev) {
            double sample = _target.as_float();
            average += (sample - average) * 0.1;
            median += copysign(average * 0.01, sample - median);
            _target.from(average);
            return true;
        }
};
// Filter JMC median macro with automatic marker expansion
#define filter_jmc_median(...) \
    IOTEMPOWER_DISPATCH_2(__VA_ARGS__, \
                     filter_jmc_median_with_marker_, \
                     filter_jmc_median_simple_)(__VA_ARGS__)

// Implementation with no parameters
#define filter_jmc_median_simple_() \
    with_filter_callback(*new Filter_JMC_Median())

// Implementation with one parameter (marker name)
#define filter_jmc_median_with_marker_(marker_name) \
    with_filter_callback(*new Filter_JMC_Median(MARKED(marker_name)))

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
        Ustring& _target;
    public:
        Filter_Restrict(double from, double to) : Callback(), _target(get_device().value()) {
            _from = from;
            _to = to;
        }
        Filter_Restrict(Ustring& target, double from, double to) : Callback(), _target(target) {
            _from = from;
            _to = to;
        }
        bool call(Device &dev) {
            double sample = _target.as_float();
            if(sample < _from || sample > _to) return false;
            return true;
        }
};
// Filter restrict macro with automatic marker expansion
#define filter_restrict(...) \
    IOTEMPOWER_DISPATCH_3(__VA_ARGS__, \
                     filter_restrict_with_marker_, \
                     filter_restrict_simple_, \
                     filter_restrict_simple_)(__VA_ARGS__)

// Implementation with two parameters (from, to)
#define filter_restrict_simple_(from, to) \
    with_filter_callback(*new Filter_Restrict(from, to))

// Implementation with three parameters (marker name, from, to)
#define filter_restrict_with_marker_(marker_name, from, to) \
    with_filter_callback(*new Filter_Restrict(MARKED(marker_name), from, to))


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
        Ustring& _target;
    public:
        Filter_Binarize(double cutoff, const char* high, const char* low) : Callback(), _target(get_device().value()) {
            _cutoff = cutoff;
            _high = high;
            _low = low;
        }
        Filter_Binarize(Ustring& target, double cutoff, const char* high, const char* low) : Callback(), _target(target) {
            _cutoff = cutoff;
            _high = high;
            _low = low;
        }
        bool call(Device &dev) {
            if(_target.equals(_low)) return false;
            if(_target.equals(_high)) return false;
            double sample = _target.as_float();
            if(sample >= _cutoff) {
                _target.from(_high);
            } else {
                _target.from(_low);
            }
            return true;
        }
};
// Filter binarize macro with automatic marker expansion
#define filter_binarize(...) \
    IOTEMPOWER_DISPATCH_4(__VA_ARGS__, \
                     filter_binarize_with_marker_, \
                     filter_binarize_simple_, \
                     filter_binarize_simple_, \
                     filter_binarize_simple_)(__VA_ARGS__)

// Implementation with three parameters (cutoff, high, low)
#define filter_binarize_simple_(cutoff, high, low) \
    with_filter_callback(*new Filter_Binarize(cutoff, high, low))

// Implementation with four parameters (marker name, cutoff, high, low)
#define filter_binarize_with_marker_(marker_name, cutoff, high, low) \
    with_filter_callback(*new Filter_Binarize(MARKED(marker_name), cutoff, high, low))


/* map set of intervals to single discrete values
 * always assumed to start at minus infinity and needs is automatically terminated by infinity
 * example parameters: "low", -0.5, NULL, 0.5, "high"
 * NULL means the interval is skipped
 * */
class Filter_Interval_Map : public Callback {
    private:
        double *borders;
        const char* *values;
        int count = 0;
        Ustring& _target;
        
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
        // Original constructors for device values
        Filter_Interval_Map(const char* v0) : Callback(), _target(get_device().value()) {
            if(_allocate(1)) {
                _add(v0, INFINITY);            
            }
        }
        Filter_Interval_Map(const char* v0, double b0, 
                            const char* v1) : Callback(), _target(get_device().value()) {
            if(_allocate(2)) {
                _add(v0, b0);            
                _add(v1, INFINITY);            
            }
        }
        Filter_Interval_Map(const char* v0, double b0, 
                            const char* v1, double b1,
                            const char* v2) : Callback(), _target(get_device().value()) {
            if(_allocate(3)) {
                _add(v0, b0);            
                _add(v1, b1);            
                _add(v2, INFINITY);            
            }
        }
        Filter_Interval_Map(const char* v0, double b0, 
                            const char* v1, double b1,
                            const char* v2, double b2,
                            const char* v3) : Callback(), _target(get_device().value()) {
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
                            const char* v4) : Callback(), _target(get_device().value()) {
            if(_allocate(5)) {
                _add(v0, b0);
                _add(v1, b1);
                _add(v2, b2);
                _add(v3, b3);
                _add(v4, INFINITY);
            }
        }
        
        // New constructors for marker values
        Filter_Interval_Map(Ustring& target, const char* v0) : Callback(), _target(target) {
            if(_allocate(1)) {
                _add(v0, INFINITY);            
            }
        }
        Filter_Interval_Map(Ustring& target, const char* v0, double b0, 
                            const char* v1) : Callback(), _target(target) {
            if(_allocate(2)) {
                _add(v0, b0);            
                _add(v1, INFINITY);            
            }
        }
        Filter_Interval_Map(Ustring& target, const char* v0, double b0, 
                            const char* v1, double b1,
                            const char* v2) : Callback(), _target(target) {
            if(_allocate(3)) {
                _add(v0, b0);            
                _add(v1, b1);            
                _add(v2, INFINITY);            
            }
        }
        Filter_Interval_Map(Ustring& target, const char* v0, double b0, 
                            const char* v1, double b1,
                            const char* v2, double b2,
                            const char* v3) : Callback(), _target(target) {
            if(_allocate(4)) {
                _add(v0, b0);
                _add(v1, b1);
                _add(v2, b2);
                _add(v3, INFINITY);
            }
        }
        Filter_Interval_Map(Ustring& target, const char* v0, double b0, 
                            const char* v1, double b1,
                            const char* v2, double b2,
                            const char* v3, double b3,
                            const char* v4) : Callback(), _target(target) {
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
            double v = _target.as_float();
            for(int i=0; i<=count; i++) { // make infinity as values included
                if(v<borders[i]) {
                    if(values[i]) {
                        _target.from(values[i]);
                        return true;
                    } else return false; // when NULL discard value
                }
            }
            return false;
        }
};
#define filter_interval_map(...) with_filter_callback(*new Filter_Interval_Map(__VA_ARGS__))
#define filter_interval_map_m(marker_name, ...) with_filter_callback(*new Filter_Interval_Map(MARKED(marker_name), ##__VA_ARGS__))

// This filter works on precise buffer
#include <TrueRMS.h>
#include <dev_input_base.h>
class Filter_Precise_RMS : public Callback {
    private:
        Rms *rms; // reference to TrueRMS object
    public:
        Filter_Precise_RMS() : Callback() {
            size_t buflen = ((Input_Base&)get_device()).get_buffer_size();
            rms = new Rms();
            // TODO: check NULL

//            rms->begin(VoltRange, buflen, ADC_12BIT, BLR_ON, CNT_SCAN);
            float VoltRange = 3.00; // TODO: might have to be adjustable
            rms->begin(VoltRange, buflen, ADC_12BIT, BLR_ON, SGL_SCAN); // 12 bit on esp32
        }
        bool call(Device &dev) {
            Input_Base& d = (Input_Base &)dev;
            int buffer_size = d.get_buffer_size();
            int buffer_fill = d.get_buffer_fill();
            int* buffer = d.get_buffer();
            if(buffer_fill == buffer_size) {
                // compute for whole buffer at once
                rms->start();
                for(int i=0; i<buffer_size; i++) {
                    rms->update(buffer[i]);
                }
                rms->publish();
                dev.value(0).printf("%.4f, %d", rms->rmsVal, rms->dcBias);
                rms->stop();
                // TODO: should buffer be reset here?
                return true;
            }
            return false;
        }
};
#define filter_precise_rms() with_filter_callback(*new Filter_Precise_RMS())
// TODO: think about other name? like energy?


#endif // _IOTEMPOWER_FILTER_H_
