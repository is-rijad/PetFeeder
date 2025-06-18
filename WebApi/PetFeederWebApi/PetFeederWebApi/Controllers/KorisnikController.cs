using Microsoft.AspNetCore.Mvc;
using PetFeederWebApi.Data;
using PetFeederWebApi.Models;
using PetFeederWebApi.Models.ViewModels;
using System.Net.Mail;
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Text.Json;
using System.Text.Unicode;
using NuGet.Protocol;
using static System.Net.Mime.MediaTypeNames;

namespace PetFeederWebApi.Controllers {
    [ApiController]
    [Route("[controller]/[action]")]
    public class KorisnikController : ControllerBase {
        private readonly PetFeederContext _context;
        IConfigurationRoot config;

        public KorisnikController(PetFeederContext context) {
            _context = context;
            config = new ConfigurationBuilder().AddJsonFile("appsettings.json", false).Build(); ;
        }

        [HttpPost]
        public IActionResult RegistrujKorisnika([FromBody] KorisnikVM korisnik) {
            var k = _context.Korisnici.Where(k => k.Email.Equals(korisnik.email)).FirstOrDefault();
            if (k != null) {
                return BadRequest("Korisnik vec postoji!");
            }
            _context.Add(new Korisnik()
            {
                Email = korisnik.email,
                Lozinka = korisnik.lozinka,
            });
            _context.SaveChanges();
            return Ok("Korisnik je uspjesno registrovan!");
        }

        [HttpPost]
        public ActionResult UlogujKorisnika([FromBody] KorisnikVM korisnik) {
            var obj = _context.Korisnici.Where(k => k.Email.Equals(korisnik.email)
            && k.Lozinka.Equals(korisnik.lozinka)).FirstOrDefault();
            if (obj == null)
                return NotFound("Korisnik ne postoji!");
            return Ok(obj);
        }

        [HttpPost]
        public ActionResult PromijeniLozinku([FromBody] PromjenaLozinkeVM lozinka)
        {
            var id = lozinka.korisnikID;
            var staraLozinka = lozinka.staraLozinka;
            var novaLozinka = lozinka.novaLozinka;

            var korisnik = _context.Korisnici.Where(k => k.KorisnikID == id).FirstOrDefault();
            if (korisnik == null)
            {
                return BadRequest("Korisnik ne postoji!");
            }

            if (!korisnik.Lozinka.Equals(staraLozinka))
            {
                return BadRequest("Stara lozinka nije ispravna!");
            }

            PostaviLozinku(id, novaLozinka);
            return Ok("Lozinka je uspjesno postavljena!");
        }


        [HttpGet] 
        public async Task<ActionResult> PosaljiEmail([FromQuery] string email) {
            if (!Helper.ValidirajEmail(email)) {
                return BadRequest("Email nije validan!");
            }

            var lista = config.GetChildren().ToList();
            var emailFrom = Encryption.Decoder(lista[3].GetSection("Email").Value);
            var passwordFrom = Encryption.Decoder(lista[3].GetSection("Password").Value);

            string lozinka = Helper.GenerisiLozinku();

            var korisnikId = _context.Korisnici.Where(k => k.Email.Equals(email)).FirstOrDefault();
            if (korisnikId == null)
                return BadRequest("Korisnik ne postoji!");

            PostaviLozinku(korisnikId.KorisnikID, Encryption.Encoder(lozinka));

            using (var SmtpClient = new SmtpClient
            {
                DeliveryMethod = SmtpDeliveryMethod.Network,
                Credentials = new System.Net.NetworkCredential(emailFrom, passwordFrom),
                Port = 587,
                Host = "smtp.office365.com",
                EnableSsl = true
            }) {
                string body = "Poštovani,\nVaša nova lozinka za pristup je: " + lozinka +
                "\nS poštovanjem,\nPet Feeder Team!";
                await SmtpClient.SendMailAsync(new MailMessage(emailFrom, email, "Reset lozinke", body));
                return Ok("Provjerite email inbox.");
            };
        }

        private bool PostaviLozinku(int korisnikId, string novaLozinka) {
            var korisnik = _context.Korisnici.Where(k => k.KorisnikID == korisnikId).FirstOrDefault();
            if (korisnik == null)
                return false;
            korisnik.Lozinka = novaLozinka;
            _context.Korisnici.Update(korisnik);
            _context.SaveChanges();
            return true;
        }
    }
}
