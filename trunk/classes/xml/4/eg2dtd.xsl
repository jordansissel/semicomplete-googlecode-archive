<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text">
</xsl:output>

<xsl:template match="/">
	<xsl:text>&lt;?xml version='1.0' encoding='UTF-8'?&gt;&#010;</xsl:text>
	<xsl:apply-templates select="//element"/>
</xsl:template>

<!--
<xsl:template match="doctype">
	<xsl:apply-templates select="//element" />
</xsl:template>
-->

<xsl:template match="//element">
	<xsl:param name="tagname" select="@tag" />
	<!-- <xsl:value-of select="@tag"/> <xsl:text>&#010;</xsl:text> -->
	<xsl:choose>
		<xsl:when test="@like">
			<xsl:variable name="newtag" select="@like"/>
			<xsl:apply-templates select="//element[@tag = $newtag]">
				<xsl:with-param name="tagname" select="$tagname"/>
			</xsl:apply-templates>
		</xsl:when>
		<xsl:when test="count(*) = 0 and string-length(text()) = 0">
			<!-- do nothing -->
		</xsl:when>
		<xsl:otherwise>
			<xsl:call-template name="element-entry">
				<xsl:with-param name="tagname" select="$tagname"/>
			</xsl:call-template>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template name="element-entry">
	<xsl:param name="node" />
	<xsl:param name="tagname" select="@tag" />
	<xsl:if test="count(*) > 0 or string-length(text()) > 0">
		<xsl:text>&lt;!ELEMENT </xsl:text>
		<xsl:value-of select="$tagname" />
		<xsl:text> </xsl:text>
		<xsl:call-template name="children" />
		<xsl:text>>&#010;</xsl:text>
		<!--<xsl:apply-templates select="*"/>-->
	</xsl:if>
</xsl:template>

<xsl:template match="alt">
	<xsl:apply-templates select="*"/>
</xsl:template>

<xsl:template name="children">
	<xsl:param name="sep" select="', '"/>
	<xsl:choose>
		<xsl:when test="count(*) = 0 and string-length(text()) = 0">
			<xsl:text>#EMPTY</xsl:text>
		</xsl:when>
		<xsl:when test="count(*) = 0">
			<xsl:text>(#PCDATA)</xsl:text>
		</xsl:when>
		<xsl:otherwise>
			<xsl:text>(</xsl:text>
			<xsl:if test="string-length(normalize-space(text())) > 0">
				<xsl:text>#PCDATA</xsl:text>
				<xsl:if test="count(*) > 0">
					<xsl:value-of select="$sep"/>
				</xsl:if>
			</xsl:if>
			<xsl:for-each select="*">
				<xsl:choose>
					<xsl:when test="name() = 'element'"> 
						<xsl:value-of select="@tag" /> 
					</xsl:when>
					<xsl:when test="name() = 'alt'"> 
						<xsl:call-template name="children">
							<xsl:with-param name="sep" select="' | '" />
						</xsl:call-template>
					</xsl:when>
				</xsl:choose>
				<xsl:if test="position() &lt; last()">
					<xsl:value-of select="$sep"/>
				</xsl:if>
			</xsl:for-each>
			<xsl:text>)</xsl:text>
			<xsl:choose>
				<xsl:when test="@repeat = 'many'"><xsl:text>+</xsl:text></xsl:when>
				<xsl:when test="@repeat = 'some'"><xsl:text>*</xsl:text></xsl:when>
			</xsl:choose>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

</xsl:stylesheet>
