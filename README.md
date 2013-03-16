RAVEN2DB
=========

Raven2DB is a simple 'c' application to read a communication port that is attached to a rainforest automation RAVEn device and store the raw XML data into a MySQL database.

  - requires MySQL-Devel packages
  - requires [rainforest] RAVEn(tm) device
  - application runs as root

This is a very simple app that I needed to just save the XML. I can not support the set up of this application on your system.  I currently just run mine as a ROOT user to stuff the data in.  If you want to run this as a different user. You will need to give access to the Communicaiton port to that user and so on.  The same is for the MySQL database, you need to give access to your database and table for this program to write to the tabel.


Device
--
[RAVEn] - rainforestautomation Zigbee 1.0 device



STEP #1 - Load [RAVEn] com port driver
-
The RAVEn device will output XML when it is plugged in. It doesn't matter if its connected to a device or not.  You will need to set up your Linux machine to install the proper driver

This device is an *ftdi-sio* device I used the standard CentOS linux driver by running the following modprobe command

    modprobe -r ftdi-sio product=0x8a28 vendor=0x0403

I checked my **dmesg** output to see that my driver /dev/ttyUSB0 was installed, I then ran the command

    cat /dev/ttyUSB0

at that point the XML output from the RAVEn device was displayed.
So I knew the driver was correct and the unit was working I decided to have the driver installed at boot time on my CentOS 6 x64 machine (ref: [Load at boot])

    echo "modprobe ftdi-sio product=0x8a28 vendor=0x0403" >/etc/sysconfig/modules/my_ftdi.modules
    chmod +x /etc/sysconfig/modules/my_ftdi.modules



STEP #2 - Create Database and tables
-
Using your favoriate MySQL tool(s) create a Database called **raven** and the **ravenxml** table.  The only field the 'C' code will insert is the *xml* field.  All the other fields you can use but will not be set on the insert


    --
    -- Table structure for table `ravenxml`
    --
    
    DROP TABLE IF EXISTS `ravenxml`;
    CREATE TABLE `ravenxml` (
      `id` bigint(20) NOT NULL AUTO_INCREMENT,
      `ts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
      `xml` text,
     `processed` enum('N','Y') DEFAULT 'N',
     PRIMARY KEY (`id`)
    ) ENGINE=MyISAM AUTO_INCREMENT=60072 DEFAULT CHARSET=latin1;
 
 
 
STEP #3 - Compile application 
-
    
    
Pull the files to a directory and run make to complie the application.  This is a simple make file so provided MySQL development is installed it should compile.

    make


STEP #4 - Install/RUN
-

I never built a install script, so I just run it where its installed. I've included a sample config script for your /etc/init.d/ directory.


    #!/bin/sh
    # description: Raven 2 Database program, Stores Raven output to MySQL
    # chkconfig: 2345 99 00
    
    case "$1" in
    'start')
        /usr/src/raven2db/raven2db &
        touch /var/lock/subsys/raven2db
        ;;
    'stop')
        killall raven2db
        rm -f /var/lock/subsys/raven2db
        ;;
    *)
        echo "Usage: $0 { start | stop }"
        ;;
    esac


License
-

Open Software License v. 3.0 ([OSL-3.0])


  [OSL-3.0]:http://opensource.org/licenses/OSL-3.0
  [Load at boot]:http://honglus.blogspot.com/2011/04/load-kernel-modules-at-boot-time-on.html
  [RAVEn]:http://www.rainforestautomation.com/raven
  

