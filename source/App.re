[@react.component]
let make = () => {
  let route = Router.useRouter();

  switch (route) {
  | Some(Home) => <Page_Counters />
  | None => <Input />
  };
};
