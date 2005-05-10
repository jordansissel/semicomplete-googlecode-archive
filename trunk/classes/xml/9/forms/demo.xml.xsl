<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" />
<xsl:attribute-set name="form">
	<xsl:attribute name="name" />
	<xsl:attribute name="method" />
	<xsl:attribute name="action" />
</xsl:attribute-set>

<xsl:attribute-set name="form-input">
	<xsl:attribute name="name" />
	<xsl:attribute name="value" />
	<xsl:attribute name="type" />
</xsl:attribute-set>

<xsl:template match="/">
	<xsl:apply-templates />
</xsl:template>

<xsl:template match="/form">
	<xsl:copy use-attribute-sets="form">
		<xsl:attribute name="name"> <xsl:value-of select="@name" /> </xsl:attribute>
		<xsl:attribute name="method"> <xsl:value-of select="@method" /> </xsl:attribute>
		<xsl:attribute name="action"> <xsl:value-of select="@action" /> </xsl:attribute>
		<xsl:apply-templates />
		<p/>
		<input type="submit" />
	</xsl:copy>

</xsl:template>

<xsl:template match="*">
	<xsl:if test="@label">
		<xsl:value-of select="@label" />:
	</xsl:if>
	<xsl:copy use-attribute-sets="form-input">
		<xsl:attribute name="type"> <xsl:value-of select="@type" /> </xsl:attribute>
		<xsl:attribute name="name"> <xsl:value-of select="@name" /> </xsl:attribute>
		<xsl:attribute name="value"> <xsl:value-of select="@value" /> </xsl:attribute>
		<xsl:apply-templates />
	</xsl:copy>
	<xsl:if test="name() = 'input' and @type != 'hidden'">
		<br/>
	</xsl:if>
</xsl:template>
</xsl:stylesheet>

