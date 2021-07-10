let baseUrl = "http://127.0.0.1:3003/";
let countersUrl = baseUrl ++ "counters";

type t = RemoteData.t(Counters.Counter_t.counters);

exception DecodeError(string);
exception ServerError(string);

let handleCounterData = data => {
  /* This has to be wrong. Will get back to it */
  let dict = Js.Dict.empty();
  Js.Dict.set(dict, "counters", data);
  let json: Js.Json.t = Js.Json.object_(dict);
  /* turn it into a proper record */
  let counters: Counter_t.counters =
    Atdgen_codec_runtime.Decode.decode(Counter_bs.read_counters, json);

  counters;
};
let fetchCounters = () =>
  Js.Promise.(
    Fetch.fetch(countersUrl)
    |> then_(res => Fetch.Response.json(res))
    |> then_(json => json |> handleCounterData |> resolve)
  );
