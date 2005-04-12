<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" />
<xsl:template match="*">
	<xsl:apply-templates select="*" />
</xsl:template>

<xsl:template match="*[@id]">
	<xsl:apply-templates select="child::*[1]" />
	<xsl:value-of select="@id" />
	<xsl:text>&#010;</xsl:text>
	<xsl:apply-templates select="child::*[2]" />
</xsl:template>

</xsl:stylesheet>
