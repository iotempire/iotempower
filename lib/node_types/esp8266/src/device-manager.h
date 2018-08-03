// device-manager.h
// author: ulno
// created: 2018-07-16
// Manage a list of devices

#ifndef _ULNOIOT_DEVICE_MANAGER_H_
#define _ULNOIOT_DEVICE_MANAGER_H_

#include <AsyncMqttClient.h>
#include <toolbox.h>
#include <device.h>

// ad a device to the static device list
bool add_device(Device& device);

/* out of simplicity reasons, we don't allow the deletion of devices
 * as we dropped interactive configuration in the transition from
 * micropython to C */
//bool remove_device(const char* name);

/* measure, filter, and check values
 * return true, when any values where updated */
bool devices_update();

/* Subscribe all devices */
bool devices_subscribe(AsyncMqttClient& mqtt_client, Ustring& node_topic);

/* match a receive-topic and deliver payload */
bool devices_receive(Ustring& subtopic, Ustring& payload);

/* check if changes have to be published
 * -> eventually publish
 * This is only called with enough time distance so send buffer doesn't run over.
 * Careful a value causing a change could already have been overwritten.
 * Return true if anything has been published.
 * Return false if nothing was published.
 */
bool devices_publish(AsyncMqttClient& mqtt_client, Ustring& node_topic, bool publish_all = false);

#endif // _ULNOIOT_DEVICE_MANAGER_H_
