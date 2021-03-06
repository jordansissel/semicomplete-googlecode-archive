#!/bin/sh
#
# Copyright 2007 Jordan Sissel
# License is BSD, which you should find attached with this package.

TMP=`mktemp /tmp/xpathtool.XXXXXX`
DOINDENT="yes"
OUTPUTMETHOD="text"
STRIPSPACE=""
PRETTY="1"

XSLTPROC_FLAGS=""

while [ $# -gt 0 ]; do
  case $1 in
    --ihtml) XSLTPROC_FLAGS="$XSLTPROC_FLAGS --html" ;;
    --otext) OUTPUTMETHOD="text" ;;
    --oxml) OUTPUTMETHOD="xml" ;;
    --ohtml) OUTPUTMETHOD="html" ;;
    --oxsl) OUTPUTMETHOD="xsl" ;;
    --indent) DOINDENT="yes" ;;
    --noindent) DOINDENT="no" ;;
    --stripspace=*) STRIPSPACE="${1%*=}" ;;
    --pretty) PRETTY="1" ;;
    --nopretty) PRETTY="" ;;
    *) break ;;
  esac
  shift
done

XPATH="$1"

# Sanitize:
XPATH=`echo "$XPATH" | sed -e 's/"/\&quot;/g'`

cat << XML \
| m4 -DXPATH="$XPATH" \
     -DDOINDENT="$DOINDENT" \
     -DSTRIPSPACE="$STRIPSPACE" \
     -DPRETTY="$PRETTY" \
     -DOUTPUTMETHOD="$OUTPUTMETHOD" > $TMP
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="OUTPUTMETHOD" indent="DOINDENT"/>
<xsl:strip-space elements="STRIPSPACE" />

<xsl:template match="/">
  <toplevel>
  <xsl:for-each select="XPATH">
    <xsl:choose>
      <xsl:when test="'OUTPUTMETHOD' = 'text'">
        <xsl:value-of select="."/> <xsl:text>&#010;</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:copy-of select="."/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:for-each>
  </toplevel>
</xsl:template>
</xsl:stylesheet>
XML

if [ "$OUTPUTMETHOD" = "xsl" ] ; then
  cat $TMP
else
  xsltproc $XSLTPROC_FLAGS $TMP -
fi \
| ([ "$OUTPUTMETHOD" != "text" -a ! -z "$PRETTY" ] && xmllint --format - || cat)

rm $TMP
