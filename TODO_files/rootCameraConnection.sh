echo 'KERNEL=="raw1394", GROUP="video", MODE="0664"' | tee /etc/udev/rules.d/50-raw1394.rules && restart udev
chmod 666 /dev/raw1394
