using System;
using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace PetFeederWebApi.Migrations
{
    /// <inheritdoc />
    public partial class RemovePosljednjiUpdateVrijemeField : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "PosljednjiUpdateVrijeme",
                table: "Uredjaji");
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<DateTime>(
                name: "PosljednjiUpdateVrijeme",
                table: "Uredjaji",
                type: "datetime2",
                nullable: true);
        }
    }
}
