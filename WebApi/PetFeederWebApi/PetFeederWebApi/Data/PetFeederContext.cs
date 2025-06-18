using Microsoft.EntityFrameworkCore;
using PetFeederWebApi.Models;
using System.Reflection.Metadata;

namespace PetFeederWebApi.Data {
    public class PetFeederContext : DbContext {
        public DbSet<Korisnik> Korisnici { get; set; }
        public DbSet<Uredjaj> Uredjaji { get; set; }
        public DbSet<Aktivacija> Aktivacije { get; set; }
        public DbSet<KorisnikUredjaj> KorisniciUredjaji { get; set; }

        public PetFeederContext(DbContextOptions options) : base(options) { }

        protected override void OnModelCreating(ModelBuilder modelBuilder) {
            modelBuilder.Entity<KorisnikUredjaj>().HasKey(pk => new { pk.KorisnikID, pk.UredjajID });
        }

    }
}
