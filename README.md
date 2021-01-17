# QuietHome
### With work and school largely taking place from home these days, QuietHome provides a simple process for getting your housemates to keep the noise down when you need a bit of peace and quiet.
## [Slides](https://docs.google.com/presentation/d/1uh7VLC4YgWx4uvmSAtE6865kep9te8QwTm802IIq6F0/edit?usp=sharing) | [Devpost](https://devpost.com/software/quiethome)
![logo](https://github.com/cterrill26/QuietHome/blob/main/images/logo.png)

## Inspiration
One of the largest changes to daily life over the past year has been that everybody does a lot more at home now, and for many that includes work and school. Of course, home is oftentimes not the best place to do such things. Surely everyone has had themselves or a colleague get interrupted during a call due to yelling from siblings, noise from the TV, or maybe a manager's son who decides his dad's meetings are the perfect opportunity to practice the saxophone every week (true story). Even worse, often when these interruptions occur, we cannot simply leave our presentations, meetings, or tests to tell the perpetrators of the noise to shush. It would be great if someone, or something, would do the shushing for us. QuietHome does exactly this!

![device](https://github.com/cterrill26/QuietHome/blob/main/images/device.jpg)

## What it does
QuietHome is an IOT smart home product that provides a network of noise-detecting devices for you to place around your home! When you have an important activity coming up and need to ensure you won't be interrupted or distracted, simply press the button on the QuietHome device in your current room. All the other devices in your home group will immediately start listening for distracting noises. If it gets too loud, they will beep and flash to notify the person responsible for the noise to quiet down. If all goes well, this will be enough to prevent distractions from coming your way. If the noise continues, a QuietHome device will send an email to everyone in the home asking them to please quiet down. To end the session, you simply press the device's button again. Congrats, your presentation/meeting/test went flawlessly! 

##Setup
Configuring your network of QuietHome devices may seem like a tricky task, but that could not be further from the truth! Our mobile application makes this as easy as can be. You simply need to register your household with a home name, email, and pin number. Then, add your QuietHome devices by their unique serial numbers. After that, you are all set up! Simply pressing the button on one device will cause all the rest to go into listening mode. You can add other people to the household by having them register under the same home name and pin number but with a different email. In the case that emails need to be sent out because somebody is causing a ruckus, everyone registered under the household will receive an email!

![register_home](https://github.com/cterrill26/QuietHome/blob/main/images/register_home.png)  ![add_device](https://github.com/cterrill26/QuietHome/blob/main/images/your_devices.png) 

## How we built it
The QuietHome device was built using an ESP32 WIFI enabled microcontroller, RGB LED, piezo transducer, push button, and a PDM microphone. The additional parts used outside of those that came in the audio & visual box were cardboard and tape for the outer structure, the inside of a ballpoint pen for pressing the button, and a small breadboard to hold the ESP32. The ESP32 was programmed using the Arduino IDE, and the Android Studio IDE was used to make the mobile application for setting up one's home network of devices. Additionally, the Realtime Database service of Google's Firebase was used for synching data between different devices and the mobile application.

![no_lid](https://github.com/cterrill26/QuietHome/blob/main/images/no_lid.jpg) 

## Challenges we ran into
The PDM microphone was a pain to get working. Fortunately, after much Googling and reading example code on GitHub, we were finally about to get it operating well enough to detect loud noises. Unfortunately, reading from the Firebase Realtime Database and reading from the microphone did not mesh well together, and doing both caused many nondeterministic errors that were only solved once we added delays between the two. Furthermore, the Android app was a challenge to design due to the complexity of the Android Studio IDE. 

## Accomplishments that we're proud of
Being able to seamlessly integrate challenging hardware and software features into a single operational product was our goal coming into this hackathon. We were able to add almost all the functionalities we had hoped for, so we are extremely happy about that. Furthermore, being able to overcome the difficulties of working in a remote Hackathon and work together to create a finished final product is something we are very proud of.

## What's next for QuietHome
We hope to integrate QuietHome with other smart home devices such as speakers, lights, TVs, and more. Imagine if when a quiet session starts, the volume for the speakers and TV lowers and the lights dim to notify everyone in the household. Additionally, we want to expand the power of our mobile application, adding features such as fine tuned control on what devices are listening for noise, and being able to set different noise thresholds for devices around the house.
