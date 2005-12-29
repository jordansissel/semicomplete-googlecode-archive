<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="xml" indent="yes"/>
<xsl:strip-space elements="*" />
<xsl:variable name="allowed-attributes">
</xsl:variable>

<xsl:template match="/article">
	<div id="article">
		<link rel="stylesheet" type="text/css" href="../article.css" />
		<xsl:apply-templates select="head" />
		<xsl:apply-templates select="body" />
	</div>
</xsl:template>

<xsl:template match="/article/head">
<!--
	<xsl:text><![CDATA[<%attr>]]>&#10;</xsl:text>
	<xsl:apply-templates select="*" />
	<xsl:text><![CDATA[</%attr>]]></xsl:text>
-->
</xsl:template>

<xsl:template match="/article/head/*">
	<xsl:value-of select="name()" /> => "<xsl:value-of select="." />"
</xsl:template>

<xsl:template match="/article/body//*[name() != 'topic']">
	<xsl:copy>
		<xsl:for-each select="@*">
			<xsl:variable name="key" select="name()" />
			<xsl:attribute name="{$key}">
				<xsl:value-of select="normalize-space(.)" />
			</xsl:attribute>
		</xsl:for-each>
		<xsl:apply-templates />
	</xsl:copy>
</xsl:template>

<xsl:template match="/article/body//topic">
	<xsl:variable name="id" select="generate-id(@title)" />
	<h4> <a name="{$id}"> <xsl:value-of select="@title" /> </a> </h4>
	<xsl:apply-templates />
</xsl:template>

<xsl:template match="/article/body">
	<span class="toc">
		<h4>Table of Contents</h4>
		<ol>
			<xsl:apply-templates mode="tocgen"/>
		</ol>
	</span>

	<xsl:apply-templates />
</xsl:template>

<!-- Table of contents Generation -->

<xsl:template match="topic" mode="tocgen">
	<xsl:variable name="id" select="generate-id(@title)" />
	<li> <a href="#{$id}"> <xsl:value-of select="@title"/> </a>
	<xsl:if test="count(.//topic) &gt; 0">
		<ul> <xsl:apply-templates select="*" mode="tocgen"/> </ul>
	</xsl:if>
	</li>
</xsl:template>

<xsl:template match="*" mode="tocgen">
	<xsl:apply-templates select="*" mode="tocgen" />
</xsl:template>

<xsl:template match="text()">
	<xsl:value-of select="." />
</xsl:template>
</xsl:stylesheet>
