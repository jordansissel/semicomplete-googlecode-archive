<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

<xsl:output method="xml" indent="yes"/>

<xsl:template match="/">
	<xsl:apply-templates select="/sitebook" />
</xsl:template>

<xsl:template match="/sitebook">
	<fo:root>
		<xsl:call-template name="layout-masters" />
		<xsl:call-template name="cover" />
		<xsl:call-template name="content" />
	</fo:root>
</xsl:template>

<xsl:template name="layout-masters">
	<fo:layout-master-set>
		<fo:simple-page-master master-name="cover" margin="1in">
			<fo:region-body margin-top="1in" margin-bottom="3in" />
			<fo:region-before extent="1.5in" />
			<fo:region-after extent="3in" />
		</fo:simple-page-master>

		<fo:simple-page-master master-name="page" margin="1in">
			<fo:region-body />
			<fo:region-before />
			<fo:region-after />
		</fo:simple-page-master>
	</fo:layout-master-set>
</xsl:template>

<xsl:template name="cover">
	<fo:page-sequence master-reference="cover">
		<fo:flow flow-name ="xsl-region-body">
			<fo:block font-size="36pt" text-align="center" font-weight="bold" margin-bottom="2in">
				<xsl:value-of select="/sitebook/cover/title" />
			</fo:block>
			<fo:block padding-top="3in">
				<xsl:call-template name="revisions" />
			</fo:block>
		</fo:flow>
	</fo:page-sequence>
</xsl:template>

<xsl:template name="revisions">
	<fo:block height=".5in" border-top="1pt solid grey" padding=".2cm">
		<fo:table>
			<fo:table-column column-width="1in" />
			<fo:table-column column-width="1in" />
			<fo:table-column column-width="1.5in" />
			<fo:table-column column-width="1in" />
			<fo:table-column column-width="1.7in" />
			<fo:table-header>
				<fo:table-row>
					<fo:table-cell border="1pt solid #444444" padding="2pt">
						<fo:block>Revision</fo:block>
					</fo:table-cell>
					<fo:table-cell border="1pt solid #444444" padding="2pt">
						<fo:block>Date</fo:block>
					</fo:table-cell>
					<fo:table-cell border="1pt solid #444444" padding="2pt">
						<fo:block>Author</fo:block>
					</fo:table-cell>
					<fo:table-cell border="1pt solid #444444" padding="2pt">
						<fo:block>Sections</fo:block>
					</fo:table-cell>
					<fo:table-cell border="1pt solid #444444" padding="2pt">
						<fo:block>Changes</fo:block>
					</fo:table-cell>
				</fo:table-row>
			</fo:table-header>
			<fo:table-body>
				<xsl:apply-templates select="/sitebook/revisions/*" />
			</fo:table-body>
		</fo:table>
	</fo:block>
</xsl:template>

<xsl:template match="/sitebook/revisions/*">
	<fo:table-row>
		<fo:table-cell border="1pt solid #444444" padding="2pt">
			<fo:block> <xsl:value-of select="version" /> </fo:block>
		</fo:table-cell>
		<fo:table-cell border="1pt solid #444444" padding="2pt">
			<fo:block> <xsl:value-of select="date" /> </fo:block>
		</fo:table-cell>
		<fo:table-cell border="1pt solid #444444" padding="2pt">
			<fo:block> <xsl:value-of select="author" /> </fo:block>
		</fo:table-cell>
		<fo:table-cell border="1pt solid #444444" padding="2pt">
			<fo:block> <xsl:value-of select="sections" /> </fo:block>
		</fo:table-cell>
		<fo:table-cell border="1pt solid #444444" padding="2pt">
			<fo:block> <xsl:value-of select="deltas" /> </fo:block>
		</fo:table-cell>
	</fo:table-row>
</xsl:template>

<xsl:template name="content">
</xsl:template>

</xsl:stylesheet>
