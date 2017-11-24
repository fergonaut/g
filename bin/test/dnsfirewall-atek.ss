{
    "connections": [
        {
            "receiver": "pcapFileWriter1",
            "sender": "netFilter1",
            "signal": "captured(GPacket*)",
            "slot": "write(GPacket*)"
        },
        {
            "receiver": "dnsProcessor1",
            "sender": "netFilter1",
            "signal": "captured(GPacket*)",
            "slot": "process(GPacket*)"
        },
        {
            "receiver": "ipFlowMgr1",
            "sender": "netFilter1",
            "signal": "captured(GPacket*)",
            "slot": "process(GPacket*)"
        },
        {
            "receiver": "tcpFlowMgr1",
            "sender": "ipFlowMgr1",
            "signal": "processed(GPacket*)",
            "slot": "process(GPacket*)"
        },
        {
            "receiver": "udpFlowMgr1",
            "sender": "ipFlowMgr1",
            "signal": "processed(GPacket*)",
            "slot": "process(GPacket*)"
        },
        {
            "receiver": "dnsFirewall1",
            "sender": "netFilter1",
            "signal": "captured(GPacket*)",
            "slot": "check(GPacket*)"
        }
    ],
    "nodes": [
        {
            "_class": "GNetFilter",
            "_x": -69,
            "_y": -297,
            "acceptVerdict": "REPEAT",
            "autoParse": true,
            "autoRead": true,
            "enabled": true,
            "mark": "1234",
            "objectName": "netFilter1",
            "queueNum": "0",
            "snapLen": "65536"
        },
        {
            "_class": "GCommand",
            "_x": -73,
            "_y": -340,
            "closeCommandList": [
                "iptables -F",
                "iptables -t nat -F",
                ""
            ],
            "objectName": "command1",
            "openCommandList": [
                "/etc/init.d/nscd restart",
                "",
                "#DHCP 서버와 클라이언트 설정 부분",
                "iptables -A INPUT -i eth1 -p udp --sport 67 --dport 68 -j ACCEPT",
                "iptables -A INPUT -i eth1 -p udp --sport 68 --dport 67 -j ACCEPT",
                "iptables -A OUTPUT -o eth1 -p udp --sport 68 --dport 67 -j ACCEPT",
                "iptables -A OUTPUT -o eth1 -p udp --sport 67 --dport 68 -j ACCEPT",
                "",
                "#NAT DNS Query를 신뢰하는 DNS로 IP와 포트를 forwarding 설정부분",
                "iptables -t nat -A PREROUTING -p udp -m udp --dport 53 -m mark --mark 1234 -j DNAT --to-destination 8.8.8.8:53",
                "iptables -t nat -A PREROUTING -p udp -m udp --dport 53                     -j NFQUEUE --queue-num 0",
                "iptables -t nat -A PREROUTING -p tcp -m tcp --dport 53 -m mark --mark 1234 -j DNAT --to-destination 8.8.8.8:53",
                "iptables -t nat -A PREROUTING -p tcp -m tcp --dport 53                     -j NFQUEUE --queue-num 0",
                "",
                "#NAT 설정 부분 내부 사설을 외부로 패킷을 forwarding 하는 정책부분",
                "iptables -t nat -A POSTROUTING -o eth0 -m mark --mark 1234 -j SNAT --to-source 192.168.1.119",
                "iptables -t nat -A POSTROUTING -o eth0                     -j NFQUEUE --queue-num 0",
                "",
                "iptables -A FORWARD -i eth1 -m mark --mark 1234 -j ACCEPT",
                "iptables -A FORWARD -i eth1 -j NFQUEUE --queue-num 0",
                "iptables -A FORWARD -o eth1 -m mark --mark 1234 -j ACCEPT",
                "iptables -A FORWARD -o eth1 -j NFQUEUE --queue-num 0",
                "",
                "#",
                "# My",
                "#",
                "iptables -A INPUT -m mark --mark 1234 -j ACCEPT",
                "iptables -A INPUT  -j NFQUEUE --queue-num 0",
                "iptables -A OUTPUT -m mark --mark 1234 -j ACCEPT",
                "iptables -A OUTPUT -j NFQUEUE --queue-num 0",
                ""
            ]
        },
        {
            "_class": "GPcapFileWriter",
            "_x": -310,
            "_y": -237,
            "dataLinkType": "Ipv4",
            "fileName": "pcap/yyyy.MM.dd hh-mm-ss-zzz.'pcap'",
            "objectName": "pcapFileWriter1",
            "snapLen": "65536"
        },
        {
            "_class": "GDnsProcessor",
            "_x": -181,
            "_y": -238,
            "objectName": "dnsProcessor1",
            "port": "53"
        },
        {
            "_class": "GIpFlowMgr",
            "_x": -65,
            "_y": -241,
            "checkInterval": "1",
            "fullTimeout": "180",
            "halfTimeout": "60",
            "objectName": "ipFlowMgr1"
        },
        {
            "_class": "GTcpFlowMgr",
            "_x": -125,
            "_y": -185,
            "checkInterval": "1",
            "finTimeout": "20",
            "fullTimeout": "180",
            "halfTimeout": "60",
            "objectName": "tcpFlowMgr1",
            "rstTimeout": "10"
        },
        {
            "_class": "GUdpFlowMgr",
            "_x": -25,
            "_y": -185,
            "checkInterval": "1",
            "fullTimeout": "180",
            "halfTimeout": "60",
            "objectName": "udpFlowMgr1"
        },
        {
            "_class": "GDnsFirewall",
            "_x": 40,
            "_y": -239,
            "checkInterval": "1",
            "dnsProcessor": "dnsProcessor1",
            "ipFlowMgr": "ipFlowMgr1",
            "objectName": "dnsFirewall1",
            "tcpFlowMgr": "tcpFlowMgr1",
            "udpFlowMgr": "udpFlowMgr1"
        }
    ]
}