<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                              xmlns:str="http://exslt.org/strings">
  <xsl:output method="xml" encoding="utf-8" indent="yes"/>

  <!-- get pubs -->
  <xsl:template match="/">
    <xsl:element name="Alexandersson">
      <!--<xsl:apply-templates select="//li"/>-->
    
      <xsl:for-each select="//li">
        <xsl:element name="{translate(preceding::h4[1], ' ', '-')}">
          <xsl:apply-templates select="."/>
        </xsl:element>
      </xsl:for-each>
    </xsl:element>
  </xsl:template>

  <!--
  <xsl:template match="h4"/>

  <xsl:template match="text()[.=' ']">
    <xsl:text>. </xsl:text>
  </xsl:template>

  <xsl:template match="a" mode="url">
    <xsl:text>[url]</xsl:text>
    <xsl:value-of select="@href"/>
    <xsl:text>[/url]</xsl:text>
  </xsl:template>
  
  <xsl:template match="li">
    <xsl:apply-templates select="a[substring(@href, string-length(@href)-3) = '.pdf']" mode="url"/>
    <xsl:apply-templates/>
  </xsl:template>
  -->
  
  <xsl:template match="b | a | i | text()">
    <!--<xsl:value-of select="translate(., '&#xa;&#xd;', '')"/>-->
  <xsl:value-of select="normalize-space(.)"/>
  <xsl:text> </xsl:text>
  </xsl:template>
  
  <xsl:template match="h4"/>
  
  <xsl:template match="li[preceding::h4[1] = 'Thesis']">
    <xsl:variable name="TypeUniversityYear" select="str:split(normalize-space(text()[2]), ', ')"/>
    <xsl:element name="Author"><xsl:value-of select="b"/></xsl:element>
    <xsl:element name="Title"><xsl:value-of select="normalize-space(a)"/></xsl:element>
    <xsl:element name="Type"><xsl:value-of select="$TypeUniversityYear[1]"/></xsl:element>
    <xsl:element name="School"><xsl:value-of select="$TypeUniversityYear[2]"/></xsl:element>
    <xsl:element name="Address"><xsl:value-of select="$TypeUniversityYear[3]"/></xsl:element>
    <xsl:element name="Month"><xsl:value-of select="str:split($TypeUniversityYear[4])[1]"/></xsl:element>
    <xsl:element name="Year"><xsl:value-of select="str:split($TypeUniversityYear[4])[2]"/></xsl:element>
  </xsl:template>

  <xsl:template match="li">
    <xsl:element name="Author"><xsl:value-of select="substring-before(normalize-space(descendant-or-self::node()), normalize-space(a[1]/text()))"/></xsl:element>
    <xsl:element name="Title"><xsl:value-of select="normalize-space(a[1])"/></xsl:element>
  </xsl:template>

  <!-- print content; output method=text removes tags -->
  <xsl:template match="@* | node()">
    <xsl:copy>
      <xsl:apply-templates select="@* | node()"/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
