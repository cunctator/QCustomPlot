#!/usr/bin/env python

#
# This script reads the documentation xml index and extracts compounds and members (functions,
# classes, enums, etc.). It generates a set of SQL commands to ./doclinks.sql which allow filling a
# database with according documentation keywords/references for automatic documentation link
# generation in the website and forum
#

from __future__ import print_function
import xml.etree.ElementTree as ET
import os, sys, re
sys.path.insert(1, os.path.join(sys.path[0], '..')) # include parent dir in search path for module import
from utilities import *

baseDir = sys.path[0]
os.chdir(baseDir)  # change current working dir to script dir

preferredDoxygenVersion = '1.8.12'
inputFilePath = "xml/index.xml"
outputFilePath = "doclinks.sql"

validKind = ('class', 'function', 'signal', 'namespace', 'variable', 'enum', 'enumvalue', 'typedef') # if you edit this, don't forget to also update SQL table creation statement below
excludeName = re.compile("^(m[A-Z]|Q_|operator\\W|~)") # entries that match this name will be completely omitted
excludeParent = re.compile("(\\.h|\\.cpp)$") # if an entry's parent name matches this, the parent name is left empty but the entry is still used

def refidToDocLink(refid):
  if '_1' in refid:
    return refid.replace('_1', '.html#')
  else:
    return refid + '.html'

def parseNode(docData, node, parentNode=None):
  kind = node.get('kind')
  if kind not in validKind: return
  name = node.find('name').text
  if excludeName.search(name): return
  if parentNode is not None:
    parentName = parentNode.find('name').text
    if parentName == name and parentNode.get('kind') == 'class': return # skip constructors, since we'd rather link to the class
    if len(docData) > 0 and docData[-1][0] == name and docData[-1][1] == parentName : return # skip overloads
    if excludeParent.search(parentName): parentName = ''
  else:
    parentName = ''
  docData.append( (name, parentName, refidToDocLink(node.get('refid')), kind) ) # (name, parent, ref, type)

def parseSpecialPageNode(docData, xmlName, docName):
  pageNode = root.find('./compound[@kind="page"][name="'+xmlName+'"]')
  if pageNode is not None: docData.append( (docName, '', refidToDocLink(pageNode.get('refid')), pageNode.get('kind')) )
  else: eprint("Warning: Couldn't find expected special page entry '{}' in xml".format(xmlName))

tree = ET.parse(inputFilePath)
root = tree.getroot()
if root.get('version') != preferredDoxygenVersion:
  eprint("Warning: Preferred doxygen version is {} but the input xml file was created by version {}.".format(preferredDoxygenVersion, root.get('version')))

docData = [] # build list of tuples (name, parent, ref, type) by traversing through documentation xml
# add regular compounds and members:
for compound in root.findall('compound'):
  parseNode(docData, compound)
  for member in compound.findall('member'):
    parseNode(docData, member, compound)
# add special entries:
parseSpecialPageNode(docData, 'thelayoutsystem', 'Layout System')
parseSpecialPageNode(docData, 'performanceimprovement', 'Performance Improvement Page')
parseSpecialPageNode(docData, 'dataselection', 'Selection Mechanism')

print("Extracted {n} valid references from xml documentation".format(n=len(docData)))

# generate SQL output:
try:
  outFile = open(outputFilePath, "w")
except OSError as e:
  printerror("Opening sql output file '{file}' failed: {error}".format(file=outputFilePath, error=e))
else:
  with outFile:
    outFile.write("""\
SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";

CREATE TABLE IF NOT EXISTS `doclinks` (
  `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `name` varchar(100) NOT NULL,
  `parent` varchar(100) NOT NULL,
  `ref` tinytext NOT NULL,
  `type` enum('class','function','signal','namespace','variable','enum','enumvalue','page','typedef') NOT NULL,
  PRIMARY KEY (`id`),
  KEY `doclinks_name` (`name`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;

TRUNCATE TABLE `doclinks`;
INSERT INTO `doclinks` (`name`, `parent`, `ref`, `type`) VALUES
""")
    outFile.write( ",\n".join( map(lambda docEntry: "('"+"', '".join(docEntry)+"')", docData) ) + ";"  )
    print("Written {size:0.0f}k bytes to SQL output '{file}'".format(size=outFile.tell()/1000, file=outFile.name))
