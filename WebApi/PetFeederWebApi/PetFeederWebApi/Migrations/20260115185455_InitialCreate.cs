using System;
using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace PetFeederWebApi.Migrations
{
    /// <inheritdoc />
    public partial class InitialCreate : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.CreateTable(
                name: "Korisnici",
                columns: table => new
                {
                    KorisnikID = table.Column<int>(type: "int", nullable: false)
                        .Annotation("SqlServer:Identity", "1, 1"),
                    Email = table.Column<string>(type: "nvarchar(max)", nullable: false),
                    Lozinka = table.Column<string>(type: "nvarchar(max)", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_Korisnici", x => x.KorisnikID);
                });

            migrationBuilder.CreateTable(
                name: "Uredjaji",
                columns: table => new
                {
                    UredjajID = table.Column<int>(type: "int", nullable: false)
                        .Annotation("SqlServer:Identity", "1, 1"),
                    Ime = table.Column<string>(type: "nvarchar(max)", nullable: false),
                    Mac = table.Column<string>(type: "nvarchar(max)", nullable: false),
                    Izbacivanja = table.Column<int>(type: "int", nullable: false),
                    ImaoObrokVrijeme = table.Column<DateTime>(type: "datetime2", nullable: true),
                    PosljednjiUpdateVrijeme = table.Column<DateTime>(type: "datetime2", nullable: true)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_Uredjaji", x => x.UredjajID);
                });

            migrationBuilder.CreateTable(
                name: "Aktivacije",
                columns: table => new
                {
                    AktivacijaID = table.Column<int>(type: "int", nullable: false)
                        .Annotation("SqlServer:Identity", "1, 1"),
                    UredjajID = table.Column<int>(type: "int", nullable: false),
                    Vrijeme = table.Column<DateTime>(type: "datetime2", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_Aktivacije", x => x.AktivacijaID);
                    table.ForeignKey(
                        name: "FK_Aktivacije_Uredjaji_UredjajID",
                        column: x => x.UredjajID,
                        principalTable: "Uredjaji",
                        principalColumn: "UredjajID",
                        onDelete: ReferentialAction.Cascade);
                });

            migrationBuilder.CreateTable(
                name: "KorisniciUredjaji",
                columns: table => new
                {
                    KorisnikID = table.Column<int>(type: "int", nullable: false),
                    UredjajID = table.Column<int>(type: "int", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_KorisniciUredjaji", x => new { x.KorisnikID, x.UredjajID });
                    table.ForeignKey(
                        name: "FK_KorisniciUredjaji_Korisnici_KorisnikID",
                        column: x => x.KorisnikID,
                        principalTable: "Korisnici",
                        principalColumn: "KorisnikID",
                        onDelete: ReferentialAction.Cascade);
                    table.ForeignKey(
                        name: "FK_KorisniciUredjaji_Uredjaji_UredjajID",
                        column: x => x.UredjajID,
                        principalTable: "Uredjaji",
                        principalColumn: "UredjajID",
                        onDelete: ReferentialAction.Cascade);
                });

            migrationBuilder.CreateIndex(
                name: "IX_Aktivacije_UredjajID",
                table: "Aktivacije",
                column: "UredjajID");

            migrationBuilder.CreateIndex(
                name: "IX_KorisniciUredjaji_UredjajID",
                table: "KorisniciUredjaji",
                column: "UredjajID");
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropTable(
                name: "Aktivacije");

            migrationBuilder.DropTable(
                name: "KorisniciUredjaji");

            migrationBuilder.DropTable(
                name: "Korisnici");

            migrationBuilder.DropTable(
                name: "Uredjaji");
        }
    }
}
