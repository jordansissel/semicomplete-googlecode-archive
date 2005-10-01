<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:xi="http://www.w3.org/2003/XInclude">

<xsl:output method="text" />
<xsl:param name="filename" select="'UNKNOWN'"/>

<xsl:template match="/">
<xsl:value-of select="$filename" /><xsl:text>: </xsl:text><xsl:apply-templates match="/dependencies/dependency" /><xsl:text>&#010;</xsl:text>
</xsl:template>

<xsl:template match="dependency">
<xsl:value-of select="@href" /><xsl:text> </xsl:text>
</xsl:template>

</xsl:stylesheet>

