#!/usr/bin/env python

import xml.etree.ElementTree
from xml.dom import minidom
import sys
import re

# parse the GDML file
root = xml.etree.ElementTree.parse(sys.argv[1]).getroot()

# look for all physvol elements
for pv in root.iter('physvol'):
    # get and parse name of position
    posname = pv.find('positionref').attrib['ref']

    posinfo = re.split(r'(\d+)in', posname.strip())
    pvname = posinfo[0] + posinfo[1]
    
    # add name to physvol
    pv.attrib['name'] = pvname

# add sensitive volume node to all Sensors
for lv in root.iter('volume'):
    if not 'Sensor' in lv.attrib['name']:
        continue

    element = xml.etree.ElementTree.SubElement(lv, 'auxiliary')
    element.set('auxtype', 'SensDet')
    element.set('auxvalue', lv.attrib['name'])

# reparse the xml to a string
rough_string = xml.etree.ElementTree.tostring(root, 'utf-8')
reparsed_string = minidom.parseString(rough_string).toprettyxml(indent = '  ', newl = '\n', encoding = 'utf-8')

# remove empty linea
final_string = '\n'.join(line for line in reparsed_string.split('\n') if line.strip())

# write to file
fout = open(sys.argv[2], 'w')
fout.write(final_string)
fout.close()
