# Here, new and test devices can be added
# before they can be added to the firmware
# create a new module and class and then
# add them in the following format in this file

from ulnoiot.devices import create_device

#def mydevice(name,*args,**kwargs):
#    return create_device("_mydevice_python_module_name",
#                         "MyDeviceClassName",name,*args,**kwargs)

def i2c_connector(name,*args,**kwargs):
    return create_device("_i2c_connector",
                         "I2cConnector",name,*args,**kwargs)

def hcsr04(name,*args,**kwargs):
    return create_device("_hcsr04",
                         "HCSR04",name,*args,**kwargs)
