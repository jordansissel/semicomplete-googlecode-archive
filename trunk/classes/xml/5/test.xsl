<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:template match="/">
	<xsl:variable name="foo" select="'testing'" />
	<xsl:apply-templates match="//element" />
</xsl:template>

<xsl:template match="element">
	<xsl:value-of select="$foo"/>
</xsl:template>
</xsl:stylesheet>
