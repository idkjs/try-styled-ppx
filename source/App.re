open UI;
let listToReactArray = items => items->Belt.List.toArray->React.array;
module H1 = [%styled.h1
  {|
  font-size: 32px;
  font-weight: 700;
  margin: 0;
|}
];

module H2 = [%styled.h2
  {|
  font-size: 25px;
  font-weight: 600;
  margin: 0;
|}
];

module Layout = [%styled.div
  {|
  width: 100%;
  max-width: 840px;
  padding: 80px 28px;
|}
];

module Card = [%styled.div
  (~background="white") => [|
    CssJs.unsafe("backgroundColor", background),
    [%css "border: 2px solid rgba(0 0 0 / 30%)"],
    [%css "border-radius: 6px"],
    [%css "width: 100%"],
    [%css "padding: 28px 40px"],
  |]
];

module Button = [%styled.button
  (~size_=`M, ~kind_=`Primary, ~fullWidth=false) => {
    [|
      fullWidth ? [%css "width: 100%"] : [%css "width: auto"],
      [%css "position: relative"],
      [%css "font: inherit"],
      [%css "color: inherit"],
      [%css "appearance: none"],
      [%css "cursor: pointer"],
      [%css "display: flex"],
      [%css "align-items: center"],
      [%css "justify-content: center"],
      [%css "text-decoration: none"],
      [%css "border-radius: 6px"],
      switch (size_) {
      | `S =>
        %css
        "padding: 2px 4px"
      | `M =>
        %css
        "padding: 8px 12px"
      | `L =>
        %css
        "padding: 14px 22px"
      },
      switch (kind_) {
      | `Primary =>
        %css
        "background: rgb(0 0 0 / 40%)"
      },
    |];
  }
];

module Input = [%styled.input
  [|[%css "font: inherit"], [%css "color: inherit"], [%css "width: 100%"]|]
];

module CounterItem = {
  [@react.component]
  let make = (~name, ~value) => {
    <Card>
      <Row distribute=`Between gap=2>
        <Text size=`L align=`Left> name </Text>
        <Row fullWidth=false gap=3>
          <Text size=`L align=`Left weight=`Bold>
            {value |> string_of_int}
          </Text>
          <Button size_=`M onClick=Js.log>
            <Text size=`L weight=`Bold> "+" </Text>
          </Button>
        </Row>
      </Row>
    </Card>;
  };
};

module NewCounter = {
  [@react.component]
  let make = () => {
    let (visible, setVisibility) = React.useState(_ => false);
    let open_ = () => setVisibility(_ => true);
    let close = () => setVisibility(_ => true);
    let handleClick = _event => {
      open_();
    };

    let handleBackdropClick = _event => {
      close();
    };

    <>
      <Button size_=`L fullWidth=true onClick=handleClick>
        <Text size=`M weight=`Bold> "Create new counter" </Text>
      </Button>
      {visible
         ? <Modal
             header={<H2> {React.string("New counter")} </H2>}
             footer={<div />}
             onBackdropClick=handleBackdropClick>
             <Input type_="text" />
           </Modal>
         : React.null}
    </>;
  };
};

module CounterList = {
  type state = Api.t;

  type actions =
    | SuccessData(Counter_t.counters)
    | ErrorData(string)
    | LoadingData;

  let updateState = (_state, action) => {
    switch (action) {
    | SuccessData(data) => RemoteData.Success(data)
    | ErrorData(err) => RemoteData.Failure(err)
    | LoadingData => RemoteData.Loading
    };
  };

  [@react.component]
  let make = () => {
    let (state, dispatch) = React.useReducer(updateState, NotFired);

    Hooks.useMount(() => {
      dispatch(LoadingData);
      Js.Promise.(
        Api.fetchCounters()
        |> then_(res => SuccessData(res) |> dispatch |> resolve)
        |> catch(err =>
             ErrorData(RemoteData.fromPromiseError(err))
             |> dispatch
             |> resolve
           )
      )
      ->ignore;
    });

    <Stack gap=2>
      <NewCounter />
      {switch (state) {
       | NotFired
       | Loading => <Text> "Loading" </Text>
       | Failure(e) => <Text> {"There has been an error: " ++ e} </Text>
       | Success(counters) =>

         counters.counters
         ->Belt.List.mapWithIndex((i, counter) =>
             <CounterItem
               key={i->string_of_int}
               name={counter.name}
               value={counter.value}
             />
           )
         ->listToReactArray;

       }}
    </Stack>;
  };
};

[@react.component]
let make = () => {
  <Align>
    <Layout>
      <H1> {React.string("Counters")} </H1>
      <Spacer bottom=4 />
      <CounterList />
    </Layout>
  </Align>;
};
