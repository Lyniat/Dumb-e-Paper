arp -a | grep -E 'espressif' | grep -oP  '\((.*?)\)' | sed 's/)//' | sed 's/(//' | tail -1
