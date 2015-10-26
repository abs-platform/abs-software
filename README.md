Android Beyond the Stratosphere
====================================================================================================

This is the main *software* repository for the **Android Beyond the Stratosphere** project,
developed at Technical University of Catalonia - UPC BarcelonaTech.

The project at a glance
----------------------------------------------------------------------------------------------------
###Vision
Space is currently restricted to governments and large corporations due to the large cost associated
to spacecraft design and launch. Although various initiatives have appeared with the vision of
democratizing the access to space such as public telescopes (ILU Telescope, Arkyd) or
open-source/crowd-funded satellites (Ardusat, Kicksat), the barrier is still too high for the
general public. The Android Beyond the Stratosphere (ABS) project, which was recipient of a Google
Faculty Research Award in the 2013 call, is aimed at providing a public access to space through the 
creation of an Open Space Station: a satellite constellation conformed by Android-based
nano-satellites which allows the execution of community-developed Android applications in space.

The OSS basic building block, the ABS Unit, encompasses both the essential hardware and software
components required to build a highly modular payload-oriented nano-satellite.

###Hardware
The ABS project addresses the design of an open-hardware platform based on commercial-of-the-shelf
(COTS) components, highly standardized designs and widely available modules. The current hardware
design is centered around a selected Android phone (Google/LG Nexus 5) and a custom Arduino board.
The Android phone is aimed at controlling the ABS unit as its centralized On-Board Computer. In
compliance with the Arduino standard, a hardware extension board has been envisioned to allow the
integration of modular payloads in the form of Arduino shields. In addition to these two basic
components, the proposed platform also encompasses a custom RF subsystem and photo-voltaic panels
that are designed with the same philosophy (COTS, widely known modules, and the like). While the
space-Arduino board provides the necessary circuitry to implement the energy management
architecture, the platform tries to reuse the available system components already available on
nowadays' smartphones (e.g. sophisticated battery charger, high-density lithium batteries.)

###Software
Complementing the hardware of the ABS Unit, this project also addresses the design of a modular
software architecture that shall allow the user community to develop Android-like apps to control
the spacecraft's payloads and access the system resources. There are three major software aspects
which this project is aimed at addressing:

- A Real-Time Operating System (RTOS) is required given the fact that space applications are
  safety-critical and require highly reliable and deterministic software. Consequently, one of the
  major axis of the ABS project is the development of an Android Real-Time Operative System (ARTOS)
  based on a Linux kernel (e.g. Hammerhead or a mainline kernel) and a RT patch (e.g. Xenomai,
  PREEMPT_RT).
- Secondly, this project also proposes the development of a modular, payload-oriented, scalable
  architecture, which shall be capable to control the spacecraft resources, instruments and
  subsystems. This architecture is proposed to be deployed on top of the ARTOS and to be tightly
  integrated with the existing Android framework (Java virtual machine, Android core, libraries and
  so on).
- Finally, the software framework of the ABS also encompasses an Satellite Development Kit
  ("PayloadSDK") which allows Android developers to seamlessly access Arduino functionality and
  devices from the app level.

An open project
----------------------------------------------------------------------------------------------------
The outcomes of the ABS project follow the Open Source Software and Open Hardware specifications and
are released in public-access repositories. Three repositiories are maintained separately with their
contents listed as follows:

- git@github.com:androidbeyondthestratosphere/abs-software.git: Software platform repository (kernel
  excluded.) Contains the source code for all the released components of the software architecture
  (including the SDBv2, Arduino firmware, Payload SDK, and others.)
- git@github.com:androidbeyondthestratosphere/artos.git: Android Real-Time Kernel repository.
  [Managed by Marc Marí].
- git@github.com:androidbeyondthestratosphere/abs-software.git: Hardware platform repository.
  Contains the source files for all the boards, mechanical parts and COTS references that will
  allow to replicate all the physical components of the ABS nano-satellite unit (including the
  custom Arduino payload control board, test boards, photo-voltaic panels, custom antenna, and the
  like).


State, contributors and community
----------------------------------------------------------------------------------------------------
The platform released in this repository (both the hardware and software parts) is currently being
developed. Every single line of code, schematic, PCB layout and report has been the result of a
collaborative effort between many graduate and undergraduate students at UPC's
Nano-Satellite and Payload Laboratory over the last 2 years. Right now, the preliminary outcomes of
this project are open to the academic, scientific and industrial community to promote engagement and
encourage collaborations. If you find this project appealing and are willing to contribute, please
do not hesitate to contact us by any means. We would love to share our experience with you and are
thrilled with the idea of community of *space-makers*.


Sponsorship
----------------------------------------------------------------------------------------------------
This project has been funded by a Google Faculty Research Award (2013 call) and by UPC
BarcelonaTech's Nano-Satellite and Payload Laboratory and Barcelona Telecommunications
School (ETSETB - TelecomBCN).
