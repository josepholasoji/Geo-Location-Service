# INTRODUCTION

Geolocation service (GS) is a high performance location service middleware. GS is written in C/C++ and its implements the Plugable API architecture and model. This  modular (and extensible) design allows for application to deploy only device(s) and functionality (s) needed. 

There is also an external signalling / processing extension support ( provided by ZeroMQ ).

This middleware designed to be of small memory footprint & usable by resource constraints devices.

The codebase is expected to compile readily on Microsoft Windows and Linux distros.

This software is liscensed under LGPL. It's is designed to be fully open source and free for personal use.
If you intend to use this product for profit; kindly contact one of our products & services personnel to ensure your success using this product, or contact on: josepholasoji@gmail.com.



# DESIGN AND ARCHITECTURE  

In this design, every GPS is modelled as a plugin (DLL for Windows & A file for Linux), each plugin is also a service (Pseudo service) with its own registered TCP port number which also serves as the service id of the same plugin. 

You can download each plugins from here. You may   also clone this project and build your binaries from source. 

<Diagram here>
