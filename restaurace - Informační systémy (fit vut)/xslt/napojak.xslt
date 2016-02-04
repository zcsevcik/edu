<?xml version="1.0" encoding="utf-8"?>
<!--
 * napojak.xslt:   Restaurace (IIS 2012)
 *
 * Author(s):      Marie Kratochvilova <xkrato11@stud.fit.vutbr.cz>
 *                 Radek Sevcik        <xsevci44@stud.fit.vutbr.cz>
 *
 * Date:           $Format:%aD$
 *
 * This file is part of iis12_restaurace.
-->

<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="html" encoding="utf-8" indent="yes"/>

  <xsl:template match="item">
    <xsl:if test="number(not(@en='false'))">
    <tr>
      <td>
        <xsl:value-of select="@nazev"/>
        <xsl:text disable-output-escaping="yes">&amp;nbsp;</xsl:text>
        <span style="font-style: italic;">
          <xsl:value-of select="@popis"/>
        </span>
      </td>
      <td nowrap="nowrap">
        <xsl:value-of select="@cena"/>
        <xsl:text>,-</xsl:text>
      </td>
    </tr>
    </xsl:if>
  </xsl:template>

  <xsl:template match="kategorie">
    <tr>
      <td colspan="2">
        <h1>
          <xsl:value-of select="@nazev"/>
          <xsl:text disable-output-escaping="yes">&amp;nbsp;</xsl:text>
          <span style="font-style: italic; font-size: medium;">
            <xsl:value-of select="@popis"/>
          </span>
        </h1>
      </td>
    </tr>
    <xsl:apply-templates select="item"/>
  </xsl:template>

  <xsl:template match="/">
    <h1>Jídelní lístek</h1>
    <div style="border:#412617 solid 1px;padding:8px 10px 4px">
      <table>
        <xsl:apply-templates select="//kategorie[item[number(not(@en='false'))]]"/>
      </table>
    </div>
  </xsl:template>

  <xsl:template match="*"/>

</xsl:stylesheet>
