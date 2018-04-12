# digitar
A DIGItal avaTAR for your Kanban/Scrum board

# Overview

The "digitar" is a digital small battery powered scrum/kanban board avatar that can be used to display different kind of information in regards to what you are working on, and other relevant information for you and your team.

The idea is that everyone in the team will have their own device, that is talking to an API hosted in the cloud. This also has a frontend that can be used by team members to update what the device will be displaying. This can be things like an avatar (picture), what issue is currently being worked on, if working from home and a custom text message etc.

The digitar will be in deep sleep most of the time to save battery power, but will wake up once every 30 minutes to fetch new data from the backend. It can also be woken up by clicking the "sync" button on the front of the device.

# Hardware
The hardware is based on an ESP8266 microcontroller and the Arduino dev environment.
We have tested the code with NodeMCU and Wemos D1 mini.

The screen is a three color 1.54" 200x200px e-paper display from Waveshare.
On the front we have a single "sync" button that will manually wake the device from deep sleep.
We also have a Li-Po battery and a charging circuit.

Everything is enclosed in a 3D-printed case, complete with a magnet for putting the device on a board.

# Software
For the microcontroller we use the Arduino dev environment.

## Backend
The backend is a simple API created using the "Serverless" framework running on AWS using lambda and dynamodb.

## Frontend
The frontend is a simple static React app hosted on S3 in AWS.
