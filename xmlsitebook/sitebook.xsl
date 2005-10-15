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
		<xsl:call-template name="revisions" />
		<xsl:call-template name="table-of-contents" />
		<xsl:call-template name="content" />
	</fo:root>
</xsl:template>

<xsl:template name="layout-masters">
	<fo:layout-master-set>
		<fo:simple-page-master master-name="cover" margin="1in" page-height="11in" page-width="8.5in">
			<fo:region-body margin-top="2in" margin-bottom="3in" />
			<fo:region-before extent="1.5in" />
			<fo:region-after extent="3in" />
		</fo:simple-page-master>

		<fo:simple-page-master master-name="revisions" margin="1in" page-height="11in" page-width="8.5in">
			<fo:region-body />
		</fo:simple-page-master>

		<fo:simple-page-master master-name="table-of-contents" margin="1in" page-height="11in" page-width="8.5in">
			<fo:region-body />
		</fo:simple-page-master>

		<xsl:apply-templates select="/sitebook/section" mode="pagelayout" />
	</fo:layout-master-set>
</xsl:template>

<xsl:template match="section" mode="pagelayout">
	<xsl:variable name="secnum" select="concat('section_', position())" />
	<fo:simple-page-master master-name="{$secnum}" margin="1in" margin-top=".5in" margin-bottom=".5in" page-height="11in" page-width="8.5in">
		<fo:region-before extent=".5in" />
		<fo:region-after extent=".5in" />
		<fo:region-body margin-top=".7in" margin-bottom=".7in" />
	</fo:simple-page-master>
</xsl:template>

<xsl:template name="cover">
	<fo:page-sequence master-reference="cover">
		<fo:static-content flow-name="xsl-region-before">
			<fo:block padding-top="8pt" padding-left="1cm" padding-right="1cm" border-top="8pt solid black" border-bottom="8pt solid black">
			<fo:table table-layout="fixed">
				<fo:table-column column-width="2in" />
				<fo:table-column column-width="4in" />
				<fo:table-body>
					<fo:table-row>
						<fo:table-cell>
							<fo:block font-size="42pt" font-family="serif" font-weight="bold" letter-spacing="3pt">
								R&#x0B7;I&#x0B7;T
							</fo:block>
						</fo:table-cell>
						<fo:table-cell>
							<fo:block font-size="18pt" text-align="center" font-weight="bold">
								Golisano College of Computing and Information Sciences
							</fo:block>
						</fo:table-cell>
					</fo:table-row>
				</fo:table-body>
			</fo:table>
			</fo:block>
		</fo:static-content>

		<!--
		<fo:static-content flow-name="xsl-region-after">
			<fo:block>
				<xsl:call-template name="revisions" />
			</fo:block>
		</fo:static-content>
		-->
		<fo:flow flow-name ="xsl-region-body">
			<xsl:apply-templates select="/sitebook/cover/*" />
			<!--
			<fo:block font-size="36pt" text-align="center" font-weight="bold" padding-bottom="2in">
				<xsl:value-of select="/sitebook/cover/title" />
			</fo:block>
			-->
		</fo:flow>
	</fo:page-sequence>
</xsl:template>

<xsl:template name="revisions">
	<fo:page-sequence master-reference="revisions">
		<fo:flow flow-name="xsl-region-body">
			<fo:block font-size="24pt">
				Revision History
			</fo:block>
			<fo:block>
				<fo:table table-layout="fixed">
					<fo:table-column column-width="1in" />
					<fo:table-column column-width="1in" />
					<fo:table-column column-width="1.5in" />
					<fo:table-column column-width="1in" />
					<fo:table-column column-width="1.7in" />
					<fo:table-header>
						<fo:table-row background-color="#D0D0D0">
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
		</fo:flow>
	</fo:page-sequence>
</xsl:template>

<xsl:template match="/sitebook/revisions/*">
	<fo:table-row>
		<fo:table-cell border=".5pt solid #444444" padding="2pt">
			<fo:block> <xsl:value-of select="version" /> </fo:block>
		</fo:table-cell>
		<fo:table-cell border=".5pt solid #444444" padding="2pt">
			<fo:block> <xsl:value-of select="date" /> </fo:block>
		</fo:table-cell>
		<fo:table-cell border=".5pt solid #444444" padding="2pt">
			<fo:block> <xsl:value-of select="author" /> </fo:block>
		</fo:table-cell>
		<fo:table-cell border=".5pt solid #444444" padding="2pt">
			<fo:block> <xsl:value-of select="sections" /> </fo:block>
		</fo:table-cell>
		<fo:table-cell border=".5pt solid #444444" padding="2pt">
			<fo:block> <xsl:value-of select="deltas" /> </fo:block>
		</fo:table-cell>
	</fo:table-row>
</xsl:template>

<xsl:template name="table-of-contents">
	<fo:page-sequence master-reference="table-of-contents">
		<fo:flow flow-name ="xsl-region-body">
			<fo:block font-size="28pt" border-bottom="2pt solid black">
				Table of Contents
			</fo:block>
			<fo:block padding-bottom=".5cm" />
			<xsl:apply-templates select="/sitebook/section" mode="tocgen" />
		</fo:flow>
	</fo:page-sequence>
</xsl:template>

<xsl:template match="section" mode="tocgen">
	<xsl:param name="parent" />
	<xsl:param name="position" select="position()" />
	<xsl:variable name="section">
		<xsl:if test="string-length($parent) > 0">
			<xsl:value-of select="$parent" /><xsl:text>.</xsl:text>
		</xsl:if>
		<xsl:value-of select="$position" />
	</xsl:variable>

	<xsl:param name="indent" select="'0'" />

	<fo:block text-align-last="justify">
		<xsl:value-of select="$section" />. 
		<fo:inline font-weight="bold">
			<xsl:value-of select="@title" />
		</fo:inline>
		<fo:leader leader-pattern="dots" />
		<fo:page-number-citation ref-id="{generate-id()}" />
		</fo:block>
	<xsl:apply-templates select="section" mode="tocgen">
		<!--<xsl:with-param name="indent" select="$indent + 1" />-->
		<xsl:with-param name="parent" select="$section" />
	</xsl:apply-templates>
</xsl:template>

<xsl:template name="content">
	<xsl:apply-templates select="/sitebook/section" mode="base"/>
</xsl:template>

<xsl:template match="/sitebook/section" mode="base">
	<xsl:variable name="secnum" select="concat('section_', position())" />
	<fo:page-sequence master-reference="{$secnum}">
		<fo:static-content flow-name="xsl-region-before">
			<fo:block text-align="right" font-size="10pt" font-variant="small-caps">
				Sections:
				<fo:retrieve-marker retrieve-class-name="section" retrieve-boundary="page" retrieve-position="first-starting-within-page" />
				<fo:leader leader-alignment="reference-area" leader-pattern="dots" />
				<fo:retrieve-marker retrieve-class-name="section" retrieve-boundary="page" retrieve-position="last-ending-within-page" />
			</fo:block>
		</fo:static-content>
		<fo:static-content flow-name="xsl-region-after">
			<xsl:call-template name="footer-revision" />
		</fo:static-content>
		<fo:flow flow-name ="xsl-region-body">
			<xsl:apply-templates select=".">
				<xsl:with-param name="parent" />
				<xsl:with-param name="position" select="position()" />
			</xsl:apply-templates>
		</fo:flow>
	</fo:page-sequence>
</xsl:template>

<xsl:template match="section">
	<xsl:param name="parent" />
	<xsl:param name="position" select="position()" />
	<xsl:variable name="section">
		<xsl:if test="string-length($parent) > 0">
			<xsl:value-of select="$parent" /><xsl:text>.</xsl:text>
		</xsl:if>
		<xsl:value-of select="$position" />
	</xsl:variable>
	<xsl:variable name="bordersize">
		<xsl:choose>
			<xsl:when test="string-length($parent) = 0">
				8pt
			</xsl:when>
			<xsl:otherwise>
				2pt
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<fo:block id="{generate-id()}" font-size="18pt" font-weight="bold" border-top="{$bordersize} solid #AAAAAA" padding-bottom="1em" padding-top="2pt">
		<xsl:value-of select="$section" />:
		<xsl:value-of select="@title" />
	</fo:block>
	<xsl:apply-templates select="*[name() != 'section']|text()"/>
	<fo:block padding-bottom="1em" font-size="8pt">
		<fo:marker marker-class-name="section">
			<!--<xsl:value-of select="concat($section,'. ', @title)" />-->
			<xsl:value-of select="$section" />
		</fo:marker>
	</fo:block>
	<xsl:apply-templates select="section">
		<xsl:with-param name="parent" select="$section" />
	</xsl:apply-templates>
</xsl:template>

<xsl:template name="footer-revision">
	 <fo:block text-align-last="justify">
		 <fo:leader leader-pattern="rule" />
	</fo:block>
	<fo:block padding-top="0pt" color="#333333" text-align-last="justify">
		<xsl:value-of select="/sitebook/cover/@title" />:
		Rev. <xsl:value-of select="/sitebook/revisions/revision/version" /> 
		(<xsl:value-of select="/sitebook/revisions/revision/author" />)
		on <xsl:value-of select="/sitebook/revisions/revision/date" />
		<fo:leader leader-pattern="space" />
		Page <fo:page-number />
	</fo:block>
</xsl:template>

<!--
  ** Special Tag Handling
  ** Special Tag Handling **
     Special Tag Handling **
                          -->
<!-- HTML-STYLE 'b' bold tag -->
<xsl:template match="b">
	<fo:inline font-weight="bold">
		<xsl:apply-templates match="*|text()"/>
	</fo:inline>
</xsl:template>

<!-- HTML-STYLE 'p' paragraph tag -->
<xsl:template match="p">
	<fo:block padding-bottom=".65em">
		<xsl:apply-templates match="*|text()"/>
	</fo:block>
</xsl:template>

<!-- HTML-STYLE 'img' image embedding tag -->
<xsl:template match="img">
	<fo:external-graphic src="url('{@src}')" />
</xsl:template>

<!-- HTML-STYLE 'ul' definition list -->
<xsl:template match="ul">
	<fo:list-block>
		<xsl:apply-templates match="li" />
	</fo:list-block>
</xsl:template>

<!-- HTML-STYLE 'li' definition list -->
<xsl:template match="li">
	<fo:list-item>
		<fo:list-item-label end-indent="label-end()">
			<fo:block><!--&#x2022;--></fo:block>
		</fo:list-item-label>
		<fo:list-item-body start-indent="body-start()">
			<fo:block>
				<xsl:apply-templates select="*|text()" />
			</fo:block>
		</fo:list-item-body>
	</fo:list-item>
</xsl:template>

<!-- HTML-STYLE 'table' stuffs -->
<xsl:template match="table">
	<xsl:variable name="border">
		<xsl:choose>
			<xsl:when test="string-length(@border) > 0">
				<xsl:value-of select="@border" />
			</xsl:when>
			<xsl:otherwise>
				1pt solid black
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<xsl:variable name="padding">
		<xsl:choose>
			<xsl:when test="string-length(@padding) > 0">
				<xsl:value-of select="@padding" />
			</xsl:when>
			<xsl:otherwise>
				2pt
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<fo:table table-layout="fixed">
		<xsl:apply-templates select="th/td" mode="table-columns" />
		<fo:table-header>
			<xsl:apply-templates select="th" />
		</fo:table-header>
		<fo:table-body>
			<xsl:apply-templates select="tr" />
		</fo:table-body>
	</fo:table>
</xsl:template>

<xsl:template match="td" mode="table-columns">
	<fo:table-column column-width="{@width}" />
</xsl:template>

<xsl:template match="th">
	<fo:table-row background-color="#D0D0D0">
		<xsl:apply-templates select="*|text()" />
	</fo:table-row>
</xsl:template>

<xsl:template match="tbody">
	<fo:table-body>
		<xsl:apply-templates select="*|text()" />
	</fo:table-body>
</xsl:template>

<xsl:template match="tr">
	<fo:table-row>
		<xsl:apply-templates select="*|text()" />
	</fo:table-row>
</xsl:template>

<xsl:template match="td">
	<!--<fo:table-cell border="{$border}" padding="{$padding}">-->
	<fo:table-cell border="1pt solid grey" padding="2pt">
		<fo:block>
			<xsl:apply-templates select="*|text()" />
		</fo:block>
	</fo:table-cell>
</xsl:template>

<!-- HTML-STYLE 'pre' white-space-loving block -->
<xsl:template match="pre">
	<fo:block white-space-collapse="false" font-family="Courier">
		<xsl:apply-templates select="text()" />
	</fo:block>
</xsl:template>

<xsl:template match="h3">
	<fo:block font-size="28pt" font-variant="small-caps">
		<xsl:apply-templates select="*|text()" />
	</fo:block>
</xsl:template>

</xsl:stylesheet>
