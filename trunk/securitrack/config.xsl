<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:template match="/xml">

<html>
<head>
<link rel="stylesheet" type="text/css" href="config.css" />
</head>
<body>

<h1>Applications</h1>
<xsl:for-each select="software/*">
	<table border="0" cellspacing="1" cellpadding="2" bgcolor="#888888" width="70%">
		<tr>
			<td colspan="4">
				<b>
					<xsl:value-of select="@name" /> 
					(<xsl:value-of select="@realname" />

					<xsl:if test="@realname">
				  		<xsl:text> </xsl:text>
					</xsl:if>	

					 <xsl:value-of select="version" />)
				</b>
			</td>
		</tr>
		<xsl:if test="count(service) &gt; 0">
			<tr>
				<td valign="top">Services:</td>
				<td valign="top" width="100%">
				<xsl:for-each select="service">
					<xsl:value-of select="." />
				   <xsl:text> </xsl:text>	
				</xsl:for-each>
				</td>
			</tr>
		</xsl:if>
		<xsl:if test="count(extra) = 1">
			<xsl:if test="count(extra/plugin) &gt; 0">
				<tr>
				<td valign="top">Plugins:</td>
				<td valign="top" width="100%">
				<xsl:for-each select="extra/plugin">
					<xsl:value-of select="@name" />
					<xsl:if test="count(requisite) &gt; 0">
						<xsl:text> - Requires: </xsl:text>
						<xsl:for-each select="requisite">
						<i>
							<xsl:value-of select="@name" />
							<xsl:text> </xsl:text>
							<xsl:value-of select="@version" />
						</i>
						</xsl:for-each>
					</xsl:if>
					<br/>
				</xsl:for-each>
				</td>
				</tr>
			</xsl:if>
		</xsl:if>
	</table>
	<br />
</xsl:for-each>

<hr />
<h1> Servers </h1>

<xsl:for-each select="servers/*">
	<table border="0" cellspacing="1" cellpadding="2" bgcolor="#888888" width="70%">
	<tr>
		<td colspan="4">
			<b> <xsl:value-of select="@name" /> </b>
		</td>
	</tr>
	<xsl:if test="count(service) &gt; 0">
		<tr>
			<td valign="top">Services: </td>
			<td valign="top" width="100%">
			<xsl:for-each select="service">
				<xsl:for-each select="provides">
					<xsl:value-of select="." /><xsl:text> </xsl:text>
				</xsl:for-each>
				<small>
					<i>
						(Provided by <xsl:value-of select="@provider" />)
					</i>
				</small>
			</xsl:for-each>
			</td>
		</tr>
	</xsl:if>
	</table>
	<br />
</xsl:for-each>

<hr />

<h1>Clusters</h1>

<xsl:for-each select="clusters/*">
	<table border="0" cellspacing="1" cellpadding="2" bgcolor="#888888" width="70%">
		<tr>
			<td colspan="4"><xsl:value-of select="@name" /></td>
		</tr>
		<tr>
			<td valign="top">Hosts</td>
			<xsl:if test="count(host) = 0">
				<td valign="top" width="100%"> No hosts found. </td>
			</xsl:if>
			<xsl:if test="count(host) &gt; 0">
				<td valign="top" width="100%">
				<xsl:for-each select="host">
					<xsl:value-of select="." /><br />
				</xsl:for-each>
				</td>
			</xsl:if>
		</tr>
	</table>
	<br />
</xsl:for-each>

</body>
</html>

</xsl:template>
</xsl:stylesheet>
