arp -a | grep -E 'ESP_CF22DE' | grep -oP  '\((.*?)\)' | sed 's/)//' | sed 's/(//'