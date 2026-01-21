using System.Net.Mail;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using PetFeederWebApi.Data;
using PetFeederWebApi.Models;
using PetFeederWebApi.Models.ViewModels;

namespace PetFeederWebApi.Controllers
{
    [ApiController]
    [Route("[controller]/[action]")]
    public class UredjajController : ControllerBase
    {
        private readonly PetFeederContext _context;
        IConfigurationRoot config;

        public UredjajController(PetFeederContext context)
        {
            _context = context;
            config = new ConfigurationBuilder().AddJsonFile("appsettings.json", false).Build();
        }

        [HttpPost]
        public async Task<IActionResult> UpdatePodataka([FromBody] UredjajVM podaci)
        {
            var uredjaj = _context.Uredjaji.FirstOrDefault(u => u.Mac == podaci.Mac);
            if (uredjaj == null)
            {
                return NotFound();
            }

            uredjaj.Izbacivanja = podaci.Izbacivanja;
            uredjaj.ImaoObrokVrijeme = podaci.ImaoObrokVrijeme;

            if (podaci.Izbacivanja == 4)
            {
                var korisnikUredjaj =
                    await _context.KorisniciUredjaji.Include(ku => ku.Korisnik).FirstOrDefaultAsync(ku => ku.UredjajID == uredjaj.UredjajID);
                await PosaljiEmail(korisnikUredjaj!.Korisnik, uredjaj.Ime);
            }
            _context.SaveChanges();
            return Ok();
        }

        [HttpPost]
        public async Task<IActionResult> ResetujIzbacivanja([FromQuery] string mac)
        {
            var uredjaj = _context.Uredjaji.FirstOrDefault(u => u.Mac == mac);
            if (uredjaj == null)
            {
                return NotFound();
            }

            uredjaj.Izbacivanja = 0;
            _context.Update(uredjaj);
            _context.SaveChanges();
            return Ok();
        }

        [HttpGet]
        public IActionResult GetPodatke([FromQuery] string mac)
        {
            var uredjaj = _context.Uredjaji.FirstOrDefault(u => u.Mac == mac);
            if (uredjaj == null)
            {
                return NotFound("Uredjaj nije pronađen!");
            }

            return Ok(new UredjajVM()
            {
                Izbacivanja = uredjaj.Izbacivanja,
                ImaoObrokVrijeme = uredjaj.ImaoObrokVrijeme,
                Mac = uredjaj.Mac
            });
        }

        private async Task PosaljiEmail(Korisnik korisnik, string hranilicaIme)
        {

            var lista = config.GetChildren().ToList();
            var emailFrom = lista[3].GetSection("Email").Value;
            var passwordFrom = lista[3].GetSection("Password").Value;


            using (var SmtpClient = new SmtpClient
            {
                DeliveryMethod = SmtpDeliveryMethod.Network,
                Credentials = new System.Net.NetworkCredential(emailFrom, passwordFrom),
                Port = 587,
                Host = "smtp.office365.com",
                EnableSsl = true
            })
            {
                string body = $"Poštovani,\nPotrebno je napuniti i resetovati {hranilicaIme} hranilicu." +
                              "\nS poštovanjem,\nPet Feeder Team!";
                await SmtpClient.SendMailAsync(new MailMessage(emailFrom, korisnik.Email,
                    $"Potrebno napuniti {hranilicaIme} hranilicu", body));

            }
        }
    }
}