using Microsoft.AspNetCore.Mvc;
using PetFeederWebApi.Data;
using PetFeederWebApi.Models.ViewModels;

namespace PetFeederWebApi.Controllers
{
    [ApiController]
    [Route("[controller]/[action]")]
    public class UredjajController : ControllerBase
    {
        private readonly PetFeederContext _context;

        public UredjajController(PetFeederContext context)
        {
            _context = context;
        }

        [HttpPost]
        public IActionResult UpdatePodataka([FromBody] UredjajVM podaci)
        {
            var uredjaj = _context.Uredjaji.FirstOrDefault(u => u.Mac == podaci.Mac);
            if (uredjaj == null)
            {
                return NotFound();
            }

            uredjaj.Izbacivanja = podaci.Izbacivanja;
            uredjaj.ImaoObrokVrijeme = podaci.ImaoObrokVrijeme;
            uredjaj.PosljednjiUpdateVrijeme = podaci.PosljednjiUpdateVrijeme;
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
    }
}