<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" />
<xsl:template match="/">
	<xsl:text>ID	PID	CID&#010;</xsl:text>
	<xsl:apply-templates select="*" />
</xsl:template>

<xsl:template match="*[@id]">
	<xsl:variable name="id" select="@id" />
	<xsl:value-of select="@id" />
	<xsl:text>	</xsl:text>
	<xsl:apply-templates select="parent::*[1]" mode="printid" />
	<xsl:text>	</xsl:text>
	<xsl:apply-templates select="child::*[1]" mode="printid" />
	<xsl:text>&#010;</xsl:text>
	<xsl:apply-templates select="*" />
</xsl:template>

<xsl:template match="*" mode="printid">
	<xsl:value-of select="@id" />
</xsl:template>

</xsl:stylesheet>
