<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" />
<xsl:template match="*">
	<xsl:apply-templates select="*" />
</xsl:template>

<xsl:template match="*[@id]">
	<xsl:value-of select="@id" />
	<xsl:apply-templates select="parent::*" mode="printid"> 
		<xsl:with-param name="texty" select="'parent'" />
	</xsl:apply-templates>
	<xsl:apply-templates select="child::*" mode="printid"> 
		<xsl:with-param name="texty" select="'child'" />
	</xsl:apply-templates>
	<xsl:text>&#010;</xsl:text>
	<xsl:apply-templates select="child::*" />
</xsl:template>

<xsl:template match="*" mode="printid">
	<xsl:param name="texty" select="'parent'" />
	<xsl:text> [</xsl:text>
	<xsl:value-of select="$texty" />
	<xsl:text>: </xsl:text>
	<xsl:value-of select="@id" />
	<xsl:text>]</xsl:text>
</xsl:template>

</xsl:stylesheet>
