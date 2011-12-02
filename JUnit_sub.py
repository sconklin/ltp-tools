#!/usr/bin/env python

#
# Generated Thu Dec  1 09:58:36 2011 by generateDS.py version 2.7a.
#

import sys

import ??? as supermod

etree_ = None
Verbose_import_ = False
(   XMLParser_import_none, XMLParser_import_lxml,
    XMLParser_import_elementtree
    ) = range(3)
XMLParser_import_library = None
try:
    # lxml
    from lxml import etree as etree_
    XMLParser_import_library = XMLParser_import_lxml
    if Verbose_import_:
        print("running with lxml.etree")
except ImportError:
    try:
        # cElementTree from Python 2.5+
        import xml.etree.cElementTree as etree_
        XMLParser_import_library = XMLParser_import_elementtree
        if Verbose_import_:
            print("running with cElementTree on Python 2.5+")
    except ImportError:
        try:
            # ElementTree from Python 2.5+
            import xml.etree.ElementTree as etree_
            XMLParser_import_library = XMLParser_import_elementtree
            if Verbose_import_:
                print("running with ElementTree on Python 2.5+")
        except ImportError:
            try:
                # normal cElementTree install
                import cElementTree as etree_
                XMLParser_import_library = XMLParser_import_elementtree
                if Verbose_import_:
                    print("running with cElementTree")
            except ImportError:
                try:
                    # normal ElementTree install
                    import elementtree.ElementTree as etree_
                    XMLParser_import_library = XMLParser_import_elementtree
                    if Verbose_import_:
                        print("running with ElementTree")
                except ImportError:
                    raise ImportError("Failed to import ElementTree from any known place")

def parsexml_(*args, **kwargs):
    if (XMLParser_import_library == XMLParser_import_lxml and
        'parser' not in kwargs):
        # Use the lxml ElementTree compatible parser so that, e.g.,
        #   we ignore comments.
        kwargs['parser'] = etree_.ETCompatXMLParser()
    doc = etree_.parse(*args, **kwargs)
    return doc

#
# Globals
#

ExternalEncoding = 'ascii'

#
# Data representation classes
#

class testsuitesSub(supermod.testsuites):
    def __init__(self, testsuite=None):
        super(testsuitesSub, self).__init__(testsuite, )
supermod.testsuites.subclass = testsuitesSub
# end class testsuitesSub


class testsuiteSub(supermod.testsuite):
    def __init__(self, tests=None, errors=None, name=None, timestamp=None, hostname=None, time=None, failures=None, properties=None, testcase=None, system_out=None, system_err=None, extensiontype_=None):
        super(testsuiteSub, self).__init__(tests, errors, name, timestamp, hostname, time, failures, properties, testcase, system_out, system_err, extensiontype_, )
supermod.testsuite.subclass = testsuiteSub
# end class testsuiteSub


class testsuiteTypeSub(supermod.testsuiteType):
    def __init__(self, tests=None, errors=None, name=None, timestamp=None, hostname=None, time=None, failures=None, properties=None, testcase=None, system_out=None, system_err=None, id=None, package=None):
        super(testsuiteTypeSub, self).__init__(tests, errors, name, timestamp, hostname, time, failures, properties, testcase, system_out, system_err, id, package, )
supermod.testsuiteType.subclass = testsuiteTypeSub
# end class testsuiteTypeSub


class propertiesTypeSub(supermod.propertiesType):
    def __init__(self, property=None):
        super(propertiesTypeSub, self).__init__(property, )
supermod.propertiesType.subclass = propertiesTypeSub
# end class propertiesTypeSub


class propertyTypeSub(supermod.propertyType):
    def __init__(self, name=None, value=None):
        super(propertyTypeSub, self).__init__(name, value, )
supermod.propertyType.subclass = propertyTypeSub
# end class propertyTypeSub


class testcaseTypeSub(supermod.testcaseType):
    def __init__(self, classname=None, name=None, time=None, error=None, failure=None):
        super(testcaseTypeSub, self).__init__(classname, name, time, error, failure, )
supermod.testcaseType.subclass = testcaseTypeSub
# end class testcaseTypeSub


class errorTypeSub(supermod.errorType):
    def __init__(self, message=None, type_=None, valueOf_=None):
        super(errorTypeSub, self).__init__(message, type_, valueOf_, )
supermod.errorType.subclass = errorTypeSub
# end class errorTypeSub


class failureTypeSub(supermod.failureType):
    def __init__(self, message=None, type_=None, valueOf_=None):
        super(failureTypeSub, self).__init__(message, type_, valueOf_, )
supermod.failureType.subclass = failureTypeSub
# end class failureTypeSub



def get_root_tag(node):
    tag = supermod.Tag_pattern_.match(node.tag).groups()[-1]
    rootClass = None
    if hasattr(supermod, tag):
        rootClass = getattr(supermod, tag)
    return tag, rootClass


def parse(inFilename):
    doc = parsexml_(inFilename)
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = 'testsuite'
        rootClass = supermod.testsuite
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
    sys.stdout.write('<?xml version="1.0" ?>\n')
    rootObj.export(sys.stdout, 0, name_=rootTag,
        namespacedef_='')
    doc = None
    return rootObj


def parseString(inString):
    from StringIO import StringIO
    doc = parsexml_(StringIO(inString))
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = 'testsuite'
        rootClass = supermod.testsuite
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
    sys.stdout.write('<?xml version="1.0" ?>\n')
    rootObj.export(sys.stdout, 0, name_=rootTag,
        namespacedef_='')
    return rootObj


def parseLiteral(inFilename):
    doc = parsexml_(inFilename)
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = 'testsuite'
        rootClass = supermod.testsuite
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
    sys.stdout.write('#from ??? import *\n\n')
    sys.stdout.write('import ??? as model_\n\n')
    sys.stdout.write('rootObj = model_.testsuite(\n')
    rootObj.exportLiteral(sys.stdout, 0, name_="testsuite")
    sys.stdout.write(')\n')
    return rootObj


USAGE_TEXT = """
Usage: python ???.py <infilename>
"""

def usage():
    print USAGE_TEXT
    sys.exit(1)


def main():
    args = sys.argv[1:]
    if len(args) != 1:
        usage()
    infilename = args[0]
    root = parse(infilename)


if __name__ == '__main__':
    #import pdb; pdb.set_trace()
    main()


