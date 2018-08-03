// device-manager.h
// author: ulno
// created: 2018-07-16
// Manage a list of devices

#ifndef _ULNOIOT_DEVICE_MANAGER_H_
#define _ULNOIOT_DEVICE_MANAGER_H_

#include <AsyncMqttClient.h>
#include <toolbox.h>
#include <device.h>

bool add_device(Device& device);
//bool remove_device(const char* name);
bool devices_measure();
bool devices_subscribe(AsyncMqttClient& mqtt_client, Ustring& node_topic);
bool devices_receive(Ustring& subtopic, Ustring& payload);

/* check if changes have to be published
 * -> eventually publish
 * This is only called with enough time distance so send buffer doesn't run over.
 * Careful a value causing a change could already have been overwritten.
 * Return true if anything has been published.
 * Return false if nothing was published.
 */
bool devices_publish(AsyncMqttClient& mqtt_client, Ustring& node_topic, bool publish_all);
bool devices_publish(AsyncMqttClient& mqtt_client, Ustring& node_topic); // publish_all = false

#endif // _ULNOIOT_DEVICE_MANAGER_H_
