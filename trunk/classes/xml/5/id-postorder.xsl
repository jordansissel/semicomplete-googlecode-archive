<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" />
<xsl:template match="/">
	<xsl:text>	ID	PREV	NEXT&#010;</xsl:text>
	<xsl:apply-templates select="*" />
</xsl:template>

<xsl:template match="*[@id]">
	<xsl:variable name="id" select="@id" />
	<xsl:apply-templates select="*">
		<xsl:with-param name="parent" select="$id" />
	</xsl:apply-templates>
	<xsl:if test="position() = 1 and not(child::*)">
		<xsl:value-of select="@id" />
	</xsl:if>
	<xsl:text>&#010;</xsl:text>
	<xsl:text>	</xsl:text>
	<xsl:value-of select="@id" />
	<xsl:text>	</xsl:text>
	<xsl:call-template name="printprev" /> 
	<xsl:text>	</xsl:text>
</xsl:template>

<xsl:template name="printprev">
	<xsl:choose>
		<xsl:when test="not(../@id)">
			<xsl:text></xsl:text>
		</xsl:when>
		<xsl:when test="position() = 1">
			<xsl:apply-templates select="parent::*[1]" mode="printid" />
		</xsl:when>
		<xsl:when test="position() = 2 and child::*">
			<xsl:apply-templates select="preceding-sibling::*[1]" mode="printid_recurse" />
		</xsl:when>
		<xsl:when test="position() = 2 and parent::*">
			<xsl:apply-templates select="preceding-sibling::*[1]" mode="printid" />
		</xsl:when>
	</xsl:choose>
</xsl:template>

<xsl:template match="*" mode="printid">
	<xsl:value-of select="@id" />
</xsl:template>

<xsl:template match="*" mode="printid_recurse">
	<xsl:choose>
		<xsl:when test="child::*[2]">
			<xsl:apply-templates select="child::*[2]" mode="printid_recurse"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="@id" />
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

</xsl:stylesheet>
