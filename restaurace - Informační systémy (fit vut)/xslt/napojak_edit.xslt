<?xml version="1.0" encoding="utf-8"?>
<!--
 * napojak_edit.xslt:   Restaurace (IIS 2012)
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
    <tr>

      <td>
        <xsl:element name="a">
            <xsl:attribute name="name">
            i_<xsl:value-of select="@id"/>
            </xsl:attribute>
        </xsl:element>
        <xsl:element name="input">
          <xsl:attribute name="type">checkbox</xsl:attribute>
          <xsl:attribute name="title">Zobrazit</xsl:attribute>
          <xsl:attribute name="name"><xsl:value-of select="@id"/></xsl:attribute>
          <xsl:if test="number(not(@en='false'))">
            <xsl:attribute name="checked">checked</xsl:attribute>
          </xsl:if>
          <xsl:attribute name="onchange">
            change_visibility(this);
          </xsl:attribute>
        </xsl:element>
      </td>
      <td>
        <xsl:element name="input">
            <xsl:attribute name="type">text</xsl:attribute>
            <xsl:attribute name="name"><xsl:value-of select="@id"/></xsl:attribute>
            <xsl:attribute name="title">Název</xsl:attribute>
            <xsl:attribute name="value"><xsl:value-of select="@nazev"/></xsl:attribute>
            <xsl:attribute name="style">width: 300px; margin-right: 5px;</xsl:attribute>
            <xsl:attribute name="onchange">
                change_item_name(this);
            </xsl:attribute>
        </xsl:element>

        <xsl:text disable-output-escaping="yes">&amp;nbsp;</xsl:text>

        <xsl:element name="input">
            <xsl:attribute name="type">text</xsl:attribute>
            <xsl:attribute name="name"><xsl:value-of select="@id"/></xsl:attribute>
            <xsl:attribute name="title">Popis</xsl:attribute>
            <xsl:attribute name="value"><xsl:value-of select="@popis"/></xsl:attribute>
            <xsl:attribute name="style">width: 50px; margin-right: 5px;</xsl:attribute>
            <xsl:attribute name="onchange">
                change_item_desc(this);
            </xsl:attribute>
        </xsl:element>
      </td>
      <td>
        <xsl:element name="input">
            <xsl:attribute name="type">text</xsl:attribute>
            <xsl:attribute name="name"><xsl:value-of select="@id"/></xsl:attribute>
            <xsl:attribute name="title">Cena</xsl:attribute>
            <xsl:attribute name="value"><xsl:value-of select="@cena"/></xsl:attribute>
            <xsl:attribute name="style">width: 30px; margin-right: 5px;</xsl:attribute>
            <xsl:attribute name="onchange">
                change_item_price(this);
            </xsl:attribute>
        </xsl:element>
        <xsl:text>,-</xsl:text>
      </td>
    </tr>
  </xsl:template>

  <xsl:template match="kategorie">
    <tr>
      <td colspan="3">
        <xsl:element name="a">
            <xsl:attribute name="name">
            k_<xsl:value-of select="@id"/>
            </xsl:attribute>
        </xsl:element>

        <xsl:element name="input">
            <xsl:attribute name="type">text</xsl:attribute>
            <xsl:attribute name="style">width: 200px; margin-right: 5px;</xsl:attribute>
            <xsl:attribute name="title">Název</xsl:attribute>
            <xsl:attribute name="name"><xsl:value-of select="@id"/></xsl:attribute>
            <xsl:attribute name="value"><xsl:value-of select="@nazev"/></xsl:attribute>
            <xsl:attribute name="onchange">
                change_kat_name(this);
            </xsl:attribute>
        </xsl:element>

        <xsl:element name="input">
            <xsl:attribute name="type">text</xsl:attribute>
            <xsl:attribute name="style">width: 300px; margin-right: 5px;</xsl:attribute>
            <xsl:attribute name="title">Popis</xsl:attribute>
            <xsl:attribute name="name"><xsl:value-of select="@id"/></xsl:attribute>
            <xsl:attribute name="value"><xsl:value-of select="@popis"/></xsl:attribute>
            <xsl:attribute name="onchange">
                change_kat_desc(this);
            </xsl:attribute>
        </xsl:element>

      </td>
    </tr>
    <xsl:apply-templates select="item"/>

    <tr>
      <td>
      </td>
      <td colspan="2">
        <xsl:element name="input">
            <xsl:attribute name="type">text</xsl:attribute>
            <xsl:attribute name="value">Nová položka</xsl:attribute>
            <xsl:attribute name="title">Přidat novou položku</xsl:attribute>
            <xsl:attribute name="style">width: 300px; margin-right: 5px; font-style: italic; color: gray;</xsl:attribute>
            <xsl:attribute name="onclick">
                this.style.color = '';
                this.style.fontStyle = '';
                this.value = '';
            </xsl:attribute>
            <xsl:attribute name="onchange">
                add_new_item(this, <xsl:value-of select="@id" />);
            </xsl:attribute>
        </xsl:element>
      </td>
    </tr>

    <tr class="spacer">
      <td colspan="3"></td>
    </tr>

  </xsl:template>

  <xsl:template match="/">
    <h1>Jídelní lístek</h1>
    <div style="border:#412617 solid 1px;padding:8px 10px 4px">
      <table>
        <tr>
        <td colspan="3">
            <xsl:element name="input">
                <xsl:attribute name="type">text</xsl:attribute>
                <xsl:attribute name="style">width: 200px; margin-right: 5px; font-style: italic; color: gray;</xsl:attribute>
                <xsl:attribute name="title">Přidat novou kategorii</xsl:attribute>
                <xsl:attribute name="value">Nová kategorie</xsl:attribute>
                <xsl:attribute name="onclick">
                    this.style.color = '';
                    this.style.fontStyle = '';
                    this.value = '';
                </xsl:attribute>
                <xsl:attribute name="onchange">
                    add_new_kat(this);
                </xsl:attribute>
            </xsl:element>
        </td>
        </tr>
        <tr class="spacer">
          <td colspan="3"></td>
        </tr>
        <xsl:apply-templates select="//kategorie"/>
      </table>
    </div>
  </xsl:template>

  <xsl:template match="*"/>

</xsl:stylesheet>
